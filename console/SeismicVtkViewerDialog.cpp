#include "SeismicVtkViewerDialog.h"
#include "ui_SeismicVtkViewerDialog.h"

#include <algorithm>
#include <array>
#include <cmath>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSignalBlocker>
#include <QSpinBox>

#include <vtkColorTransferFunction.h>
#include <vtkImageData.h>
#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkProp3D.h>
#include <vtkPropCollection.h>
#include <vtkRendererCollection.h>
#include <vtkScalarsToColors.h>

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

// The worker installs one vtkImageSlice per axis. Identify each by its
// mapper's SliceOrientation (0=X, 1=Y, 2=Z) and return the one that matches.
vtkImageSlice* findSliceForAxis(vtkRenderer* renderer, int axis)
{
    if (renderer == nullptr) return nullptr;
    vtkPropCollection* props = renderer->GetViewProps();
    if (props == nullptr) return nullptr;
    props->InitTraversal();
    while (vtkProp* prop = props->GetNextProp())
    {
        auto slice = vtkImageSlice::SafeDownCast(prop);
        if (slice == nullptr) continue;
        auto mapper = vtkImageSliceMapper::SafeDownCast(slice->GetMapper());
        if (mapper == nullptr) continue;
        if (mapper->GetOrientation() == axis)
        {
            return slice;
        }
    }
    return nullptr;
}

void extentForAxis(vtkImageData* image, int axis, int& lo, int& hi)
{
    int ext[6];
    image->GetExtent(ext);
    lo = ext[axis * 2];
    hi = ext[axis * 2 + 1];
}

// Converts a 0–100% slider value into the matching integer slice index along
// the given axis, clamped to the data's extent.
int percentToSliceIndex(vtkImageData* image, int axis, int percent)
{
    int lo = 0, hi = 0;
    extentForAxis(image, axis, lo, hi);
    const double t = std::max(0, std::min(100, percent)) / 100.0;
    const int idx = lo + static_cast<int>(std::round(t * (hi - lo)));
    return std::max(lo, std::min(hi, idx));
}

// Fetches the color transfer function driving the slices (all three slices
// share the same vtkImageProperty, so any one's LUT is the same function).
vtkColorTransferFunction* findColorTransfer(vtkRenderer* renderer)
{
    for (int axis = 0; axis < 3; ++axis)
    {
        if (auto slice = findSliceForAxis(renderer, axis))
        {
            if (auto prop = slice->GetProperty())
            {
                return vtkColorTransferFunction::SafeDownCast(prop->GetLookupTable());
            }
        }
    }
    return nullptr;
}

// Rebuilds the blue → white → red seismic ramp between [min, max] in place,
// preserving the pointer so anything referencing it (scalar bar, slice
// properties) updates automatically on the next render.
void applySeismicColorMap(vtkColorTransferFunction* lut, double min, double max)
{
    if (lut == nullptr || !(max > min)) return;
    const double mid = 0.5 * (min + max);
    const int steps = 8;

    lut->RemoveAllPoints();
    lut->SetColorSpaceToRGB();
    for (int i = 0; i <= steps; ++i)
    {
        const double t = static_cast<double>(i) / steps;
        const double val = min + t * (mid - min);
        lut->AddRGBPoint(val, t, t, 1.0);
    }
    for (int i = 1; i <= steps; ++i)
    {
        const double t = static_cast<double>(i) / steps;
        const double val = mid + t * (max - mid);
        lut->AddRGBPoint(val, 1.0, 1.0 - t, 1.0 - t);
    }
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

    // One (checkbox, spinbox) pair per axis. The checkbox drives visibility
    // of that axis's slice; the spinbox drives its slice index.
    const std::array<QCheckBox*, 3> axisCheckBoxes = {{
        ui->sliceXCheckBox, ui->sliceYCheckBox, ui->sliceZCheckBox,
    }};
    const std::array<QSpinBox*, 3> axisSpinBoxes = {{
        ui->sliceXSpinBox, ui->sliceYSpinBox, ui->sliceZSpinBox,
    }};

    for (int axis = 0; axis < 3; ++axis)
    {
        QCheckBox* checkBox = axisCheckBoxes[axis];
        QSpinBox* spinBox = axisSpinBoxes[axis];

        connect(checkBox, &QCheckBox::toggled, this, [this, axis, spinBox](bool checked) {
            if (m_renderWindow == nullptr) return;
            vtkRenderer* renderer = firstRenderer(m_renderWindow);
            if (renderer == nullptr) return;
            vtkImageSlice* slice = findSliceForAxis(renderer, axis);
            if (slice == nullptr) return;

            slice->SetVisibility(checked);

            // Apply the spinbox's current position when we turn on a slice so
            // its position reflects the UI state the user can see.
            if (checked)
            {
                if (auto mapper = vtkImageSliceMapper::SafeDownCast(slice->GetMapper()))
                {
                    if (vtkImageData* image = mapper->GetInput())
                    {
                        mapper->SetSliceNumber(percentToSliceIndex(image, axis, spinBox->value()));
                    }
                }
            }

            spinBox->setEnabled(checked);
            m_renderWindow->Render();
        });

        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, axis](int value) {
            if (m_renderWindow == nullptr) return;
            vtkRenderer* renderer = firstRenderer(m_renderWindow);
            if (renderer == nullptr) return;
            vtkImageSlice* slice = findSliceForAxis(renderer, axis);
            if (slice == nullptr) return;

            auto mapper = vtkImageSliceMapper::SafeDownCast(slice->GetMapper());
            if (mapper == nullptr) return;
            vtkImageData* image = mapper->GetInput();
            if (image == nullptr) return;

            mapper->SetSliceNumber(percentToSliceIndex(image, axis, value));
            m_renderWindow->Render();
        });
    }

    // Color range inputs: two double spinboxes drive the min/max of the
    // seismic color ramp. We rebuild the color transfer function in place so
    // the shared scalar bar and slice properties observe the change on the
    // next render.
    auto rebuildColorRange = [this]() {
        if (m_renderWindow == nullptr) return;
        const double lo = ui->colorMinDoubleSpinBox->value();
        const double hi = ui->colorMaxDoubleSpinBox->value();
        if (!(hi > lo)) return;

        vtkRenderer* renderer = firstRenderer(m_renderWindow);
        if (renderer == nullptr) return;
        vtkColorTransferFunction* lut = findColorTransfer(renderer);
        if (lut == nullptr) return;

        applySeismicColorMap(lut, lo, hi);
        m_renderWindow->Render();
    };
    connect(ui->colorMinDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [rebuildColorRange](double) { rebuildColorRange(); });
    connect(ui->colorMaxDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [rebuildColorRange](double) { rebuildColorRange(); });

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

    // Apply the initial checkbox state (set in the .ui) to the scene. This
    // makes whichever axes are checked by default visible on first show,
    // without requiring the user to toggle them. Also seed the color-range
    // spinboxes from the colormap the worker built so the user starts with
    // the actual data range (not the placeholder values in the .ui).
    if (vtkRenderer* renderer = firstRenderer(m_renderWindow))
    {
        for (int axis = 0; axis < 3; ++axis)
        {
            const bool checked = axisCheckBoxes[axis]->isChecked();
            axisSpinBoxes[axis]->setEnabled(checked);
            if (vtkImageSlice* slice = findSliceForAxis(renderer, axis))
            {
                slice->SetVisibility(checked);
                if (checked)
                {
                    if (auto mapper = vtkImageSliceMapper::SafeDownCast(slice->GetMapper()))
                    {
                        if (vtkImageData* image = mapper->GetInput())
                        {
                            mapper->SetSliceNumber(percentToSliceIndex(image, axis, axisSpinBoxes[axis]->value()));
                        }
                    }
                }
            }
        }

        if (vtkColorTransferFunction* lut = findColorTransfer(renderer))
        {
            double range[2] = {0.0, 1.0};
            lut->GetRange(range);
            QSignalBlocker blockMin(ui->colorMinDoubleSpinBox);
            QSignalBlocker blockMax(ui->colorMaxDoubleSpinBox);
            ui->colorMinDoubleSpinBox->setValue(range[0]);
            ui->colorMaxDoubleSpinBox->setValue(range[1]);
        }
    }
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
