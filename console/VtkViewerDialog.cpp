#include "VtkViewerDialog.h"
#include "ui_VtkViewerDialog.h"

#include <vtkRendererCollection.h>
#include <vtkActor.h>
#include <vtkPlane.h>
#include <vtkMapper.h>
#include <vtkDataSetMapper.h>

namespace syntheticSeismic {
namespace widgets {

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
        // Initialize bounds on first use
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
                            actor->GetMapper()->GetBounds(m_bounds);
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
            // Remove clipping planes
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

        // Calculate slice position based on bounds
        const double t = value / 100.0;
        const int coordIndex = axisIndex - 1; // 0=X, 1=Y, 2=Z
        const double position = m_bounds[coordIndex * 2] + t * (m_bounds[coordIndex * 2 + 1] - m_bounds[coordIndex * 2]);

        // Create clipping plane that cuts away geometry before the slice position
        auto plane = vtkSmartPointer<vtkPlane>::New();
        if (coordIndex == 0) // X axis
        {
            plane->SetNormal(-1.0, 0.0, 0.0);
            plane->SetOrigin(position, 0.0, 0.0);
        }
        else if (coordIndex == 1) // Y axis
        {
            plane->SetNormal(0.0, -1.0, 0.0);
            plane->SetOrigin(0.0, position, 0.0);
        }
        else // Z axis (inverted normal because Z was inverted)
        {
            plane->SetNormal(0.0, 0.0, 1.0);
            plane->SetOrigin(0.0, 0.0, position);
        }

        // Apply clipping plane to all actors
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

VtkViewerDialog::~VtkViewerDialog()
{
    delete ui;
}


}
}
