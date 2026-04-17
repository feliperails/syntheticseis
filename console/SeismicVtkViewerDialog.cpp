#include "SeismicVtkViewerDialog.h"
#include "ui_SeismicVtkViewerDialog.h"

#include <algorithm>
#include <cmath>

#include <QSignalBlocker>

#include <vtkRendererCollection.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageData.h>
#include <vtkProp3D.h>
#include <vtkPropCollection.h>

namespace syntheticSeismic {
namespace widgets {

namespace {

vtkRenderer* firstRenderer(vtkGenericOpenGLRenderWindow* window)
{
    if (window == nullptr) return nullptr;
    vtkRendererCollection* renderers = window->GetRenderers();
    if (renderers == nullptr) return nullptr;
    renderers->InitTraversal();
    return renderers->GetNextItem();
}

// The single vtkImageSlice actor the worker installs.
vtkImageSlice* findImageSlice(vtkRenderer* renderer)
{
    if (renderer == nullptr) return nullptr;
    vtkPropCollection* props = renderer->GetViewProps();
    if (props == nullptr) return nullptr;
    props->InitTraversal();
    while (vtkProp* prop = props->GetNextProp())
    {
        if (auto slice = vtkImageSlice::SafeDownCast(prop))
        {
            return slice;
        }
    }
    return nullptr;
}

// The outline wireframe actor the worker installs alongside the slice.
vtkActor* findOutlineActor(vtkRenderer* renderer)
{
    if (renderer == nullptr) return nullptr;
    vtkActorCollection* actors = renderer->GetActors();
    if (actors == nullptr) return nullptr;
    actors->InitTraversal();
    return actors->GetNextActor();
}

void extentForAxis(vtkImageData* image, int axis, int& lo, int& hi)
{
    int ext[6];
    image->GetExtent(ext);
    lo = ext[axis * 2];
    hi = ext[axis * 2 + 1];
}

}

SeismicVtkViewerDialog::SeismicVtkViewerDialog(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow, const double& initialZoomFactorZ, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SeismicVtkViewerDialog),
    m_renderWindow(renderWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

    ui->vtkWidget->setRenderWindow(m_renderWindow);
    ui->zoomZDoubleSpinBox->setValue(initialZoomFactorZ);

    connect(ui->zoomZDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](const double value) -> void {
        if (m_renderWindow == nullptr) return;

        vtkRenderer* renderer = firstRenderer(m_renderWindow);
        if (renderer == nullptr) return;

        vtkPropCollection* props = renderer->GetViewProps();
        if (props == nullptr) return;
        props->InitTraversal();
        while (vtkProp* prop = props->GetNextProp())
        {
            if (auto prop3D = vtkProp3D::SafeDownCast(prop))
            {
                prop3D->SetScale(1.0, 1.0, value);
            }
        }

        renderer->ResetCamera();
        m_renderWindow->Render();
    });

    connect(ui->sliceAxisComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](const int index) -> void {
        // Enable/disable the position spinbox first so the UI can't get
        // stuck disabled even if VTK state is unexpected.
        ui->slicePositionSpinBox->setEnabled(index > 0);
        if (index > 0)
        {
            QSignalBlocker block(ui->slicePositionSpinBox);
            ui->slicePositionSpinBox->setValue(50);
        }

        if (m_renderWindow == nullptr) return;

        vtkRenderer* renderer = firstRenderer(m_renderWindow);
        if (renderer == nullptr) return;

        vtkImageSlice* slice = findImageSlice(renderer);
        vtkActor* outline = findOutlineActor(renderer);

        if (index == 0) // None
        {
            if (slice) slice->SetVisibility(false);
            if (outline) outline->SetVisibility(true);
            m_renderWindow->Render();
            return;
        }

        if (slice == nullptr) return;

        auto mapper = vtkImageSliceMapper::SafeDownCast(slice->GetMapper());
        if (mapper == nullptr) return;
        vtkImageData* image = mapper->GetInput();
        if (image == nullptr) return;

        const int axis = index - 1; // 0 = X, 1 = Y, 2 = Z
        int lo = 0, hi = 0;
        extentForAxis(image, axis, lo, hi);
        const int midSlice = lo + (hi - lo) / 2;

        if (axis == 0) mapper->SetOrientationToX();
        else if (axis == 1) mapper->SetOrientationToY();
        else               mapper->SetOrientationToZ();
        mapper->SetSliceNumber(midSlice);

        slice->SetVisibility(true);
        // Hide the outline in slice mode — the user wanted just the selected
        // slice on screen, not the surrounding volume box.
        if (outline) outline->SetVisibility(false);

        m_renderWindow->Render();
    });

    connect(ui->slicePositionSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](const int value) -> void {
        if (m_renderWindow == nullptr) return;

        const int axisIndex = ui->sliceAxisComboBox->currentIndex();
        if (axisIndex == 0) return;

        vtkRenderer* renderer = firstRenderer(m_renderWindow);
        if (renderer == nullptr) return;
        vtkImageSlice* slice = findImageSlice(renderer);
        if (slice == nullptr) return;
        auto mapper = vtkImageSliceMapper::SafeDownCast(slice->GetMapper());
        if (mapper == nullptr) return;
        vtkImageData* image = mapper->GetInput();
        if (image == nullptr) return;

        const int axis = axisIndex - 1;
        int lo = 0, hi = 0;
        extentForAxis(image, axis, lo, hi);
        const double t = std::max(0, std::min(100, value)) / 100.0;
        const int sliceNum = lo + static_cast<int>(std::round(t * (hi - lo)));
        mapper->SetSliceNumber(std::max(lo, std::min(hi, sliceNum)));

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

void SeismicVtkViewerDialog::done(int result)
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

SeismicVtkViewerDialog::~SeismicVtkViewerDialog()
{
    if (m_renderWindow) {
        m_renderWindow->Finalize();
    }
    delete ui;
}


}
}
