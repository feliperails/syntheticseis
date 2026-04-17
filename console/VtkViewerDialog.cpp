#include "VtkViewerDialog.h"
#include "ui_VtkViewerDialog.h"

#include <vtkRendererCollection.h>
#include <vtkActor.h>
#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkClipDataSet.h>
#include <vtkDataObject.h>
#include <vtkDataSet.h>
#include <vtkPlane.h>
#include <vtkMapper.h>
#include <vtkDataSetMapper.h>

namespace syntheticSeismic {
namespace widgets {

namespace {

// Walks the mapper's input pipeline until it finds a vtkClipDataSet. Returns
// the attached vtkPlane or nullptr. Used so the slicing slot in VtkViewerDialog
// can cut through the volume (RegularGridWorker pipeline) rather than just
// clipping the outer hull via vtkMapper::AddClippingPlane (Eclipse pipeline).
vtkPlane* findClipPlane(vtkMapper* mapper)
{
    if (mapper == nullptr) return nullptr;

    vtkAlgorithm* alg = mapper->GetInputAlgorithm();
    while (alg != nullptr)
    {
        if (auto clip = vtkClipDataSet::SafeDownCast(alg))
        {
            return vtkPlane::SafeDownCast(clip->GetClipFunction());
        }
        if (alg->GetNumberOfInputPorts() == 0) break;
        vtkAlgorithm* upstream = alg->GetInputAlgorithm(0, 0);
        if (upstream == alg) break;
        alg = upstream;
    }
    return nullptr;
}

// Returns the bounds of the original (unclipped) dataset feeding into the
// mapper. Needed because mapper->GetBounds() reflects the currently clipped
// output, which shrinks as the slider moves.
bool getSourceBounds(vtkMapper* mapper, double bounds[6])
{
    if (mapper == nullptr) return false;

    vtkAlgorithm* alg = mapper->GetInputAlgorithm();
    vtkAlgorithm* lastWithData = nullptr;
    while (alg != nullptr)
    {
        if (alg->GetNumberOfInputPorts() == 0)
        {
            lastWithData = alg;
            break;
        }
        vtkAlgorithm* upstream = alg->GetInputAlgorithm(0, 0);
        if (upstream == nullptr || upstream == alg) break;
        alg = upstream;
    }

    if (lastWithData != nullptr)
    {
        lastWithData->Update();
        if (auto ds = vtkDataSet::SafeDownCast(lastWithData->GetOutputDataObject(0)))
        {
            ds->GetBounds(bounds);
            return true;
        }
    }

    mapper->GetBounds(bounds);
    return true;
}

}

VtkViewerDialog::VtkViewerDialog(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow, const double& initialZoomFactorZ, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VtkViewerDialog),
    m_renderWindow(renderWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

    ui->vtkWidget->setRenderWindow(m_renderWindow);
    ui->zoomZDoubleSpinBox->setValue(initialZoomFactorZ);

    connect(ui->zoomZDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](const double value) -> void {

        if (m_renderWindow == nullptr) return;

        vtkRendererCollection* const& renderers = m_renderWindow->GetRenderers();
        renderers->InitTraversal();

        vtkRenderer* renderer = nullptr;
        while ((renderer = renderers->GetNextItem()) != nullptr)
        {
            vtkActorCollection* const& actors = renderer->GetActors();
            actors->InitTraversal();

            vtkActor* actor = nullptr;
            while ((actor = actors->GetNextActor()) != nullptr)
            {
                actor->SetScale(1.0, 1.0, value);
            }

            renderer->ResetCamera();
        }

        m_renderWindow->Render();
    });

    connect(ui->sliceAxisComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](const int index) -> void {
        // Initialize bounds on first use, reading from the source dataset so
        // interactive slicing (which shrinks mapper->GetBounds) uses a stable
        // reference range.
        if (!m_boundsInitialized && m_renderWindow != nullptr)
        {
            vtkRendererCollection* const& renderers = m_renderWindow->GetRenderers();
            if (renderers != nullptr)
            {
                renderers->InitTraversal();
                vtkRenderer* renderer = renderers->GetNextItem();

                if (renderer != nullptr)
                {
                    vtkActorCollection* const& actors = renderer->GetActors();
                    if (actors != nullptr)
                    {
                        actors->InitTraversal();
                        vtkActor* actor = actors->GetNextActor();

                        if (actor != nullptr && actor->GetMapper() != nullptr)
                        {
                            getSourceBounds(actor->GetMapper(), m_bounds);
                            m_boundsInitialized = true;
                        }
                    }
                }
            }
        }
        // Enable/disable slider based on selection
        ui->slicePositionSlider->setEnabled(index > 0);

        if (index == 0) // None
        {
            // Restore unclipped state: remove hull clipping planes (Eclipse
            // path) and move any vtkClipDataSet plane back outside the data
            // bounds so the filter keeps every cell (RegularGrid path).
            if (m_renderWindow == nullptr) return;

            vtkRendererCollection* const& renderers = m_renderWindow->GetRenderers();
            renderers->InitTraversal();

            vtkRenderer* renderer = nullptr;
            while ((renderer = renderers->GetNextItem()) != nullptr)
            {
                vtkActorCollection* const& actors = renderer->GetActors();
                actors->InitTraversal();

                vtkActor* actor = nullptr;
                while ((actor = actors->GetNextActor()) != nullptr)
                {
                    vtkMapper* mapper = actor->GetMapper();
                    if (mapper != nullptr)
                    {
                        mapper->RemoveAllClippingPlanes();

                        if (vtkPlane* plane = findClipPlane(mapper))
                        {
                            double src[6];
                            getSourceBounds(mapper, src);
                            plane->SetNormal(-1.0, 0.0, 0.0);
                            plane->SetOrigin(src[1] + 1.0, 0.0, 0.0);
                        }
                    }
                }
            }

            m_renderWindow->Render();
        }
        else
        {
            // Reset slider to middle position when changing axis
            ui->slicePositionSlider->setValue(50);
        }
    });

    connect(ui->slicePositionSlider, &QSlider::valueChanged, this, [this](const int value) -> void {
        if (m_renderWindow == nullptr) return;

        const int axisIndex = ui->sliceAxisComboBox->currentIndex();
        if (axisIndex == 0) return; // None selected

        const double t = value / 100.0;
        const int coordIndex = axisIndex - 1; // 0=X, 1=Y, 2=Z
        const double position = m_bounds[coordIndex * 2] + t * (m_bounds[coordIndex * 2 + 1] - m_bounds[coordIndex * 2]);

        double nx = 0.0, ny = 0.0, nz = 0.0;
        double ox = 0.0, oy = 0.0, oz = 0.0;
        if (coordIndex == 0) // X axis
        {
            nx = -1.0; ox = position;
        }
        else if (coordIndex == 1) // Y axis
        {
            ny = -1.0; oy = position;
        }
        else // Z axis (Z is negated by the worker, so keep its positive side)
        {
            nz = 1.0; oz = position;
        }

        // Walk every actor. For mappers backed by a vtkClipDataSet (RegularGrid
        // pipeline), move the filter's plane so the cut face generates real
        // interior geometry. For plain mappers (Eclipse pipeline), fall back to
        // vtkMapper::AddClippingPlane, which just clips the outer hull.
        vtkRendererCollection* const& renderers = m_renderWindow->GetRenderers();
        renderers->InitTraversal();

        vtkRenderer* renderer = nullptr;
        while ((renderer = renderers->GetNextItem()) != nullptr)
        {
            vtkActorCollection* const& actors = renderer->GetActors();
            actors->InitTraversal();

            vtkActor* actor = nullptr;
            while ((actor = actors->GetNextActor()) != nullptr)
            {
                vtkMapper* mapper = actor->GetMapper();
                if (mapper == nullptr) continue;

                mapper->RemoveAllClippingPlanes();

                if (vtkPlane* filterPlane = findClipPlane(mapper))
                {
                    filterPlane->SetNormal(nx, ny, nz);
                    filterPlane->SetOrigin(ox, oy, oz);
                }
                else
                {
                    auto plane = vtkSmartPointer<vtkPlane>::New();
                    plane->SetNormal(nx, ny, nz);
                    plane->SetOrigin(ox, oy, oz);
                    mapper->AddClippingPlane(plane);
                }
            }
        }

        m_renderWindow->Render();
    });

    connect(ui->resetCameraPushButton, &QPushButton::clicked, this, [this]() -> void {
        if (m_renderWindow == nullptr) return;

        vtkRendererCollection* const& renderers = m_renderWindow->GetRenderers();
        renderers->InitTraversal();

        vtkRenderer* renderer = nullptr;
        while ((renderer = renderers->GetNextItem()) != nullptr)
        {
            renderer->ResetCamera();
        }

        m_renderWindow->Render();
    });
}

void VtkViewerDialog::done(int result)
{
    // Finalize VTK resources here, while the dialog is still visible and the OpenGL context is
    // fully valid. By the time the destructor runs, the dialog is already hidden and on Windows
    // the HDC can be invalidated, causing MakeCurrent() to fail inside Finalize().
    if (m_renderWindow) {
        m_renderWindow->Finalize();
        m_renderWindow = nullptr;
    }
    QDialog::done(result);
}

VtkViewerDialog::~VtkViewerDialog()
{
    if (m_renderWindow) {
        m_renderWindow->Finalize();
    }
    delete ui;
}


}
}
