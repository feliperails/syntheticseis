#pragma once

#include <QDialog>

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

namespace Ui {
    class SeismicVtkViewerDialog;
}

namespace syntheticSeismic {
namespace widgets {


// Viewer for the seismic (RegularGrid-backed) amplitude / depth / lithology
// volumes produced by RegularGridWorker. Separate from VtkViewerDialog, which
// continues to serve the Eclipse hex-grid path (EclipseGridWorker). Keep the
// two pipelines separate so specialisations of one don't affect the other.
class SeismicVtkViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SeismicVtkViewerDialog(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow,
                                    const double& initialZoomFactorZ,
                                    vtkSmartPointer<vtkImageData> fullImageData = nullptr,
                                    QWidget *parent = nullptr);
    ~SeismicVtkViewerDialog();

protected:
    void done(int result) override;
    void showEvent(QShowEvent* event) override;

private:
    Ui::SeismicVtkViewerDialog *ui;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    // The full (un-trimmed) ImageData. The mappers initially point at the
    // trimmed view built by the worker; unticking the "Trim empty slices"
    // checkbox swaps them to this one. Null if the worker didn't expose a
    // full view (no swap possible; checkbox is hidden).
    vtkSmartPointer<vtkImageData> m_fullImageData;
    // The trimmed ImageData captured at construction from the mapper's
    // input. Used to swap back when the user re-enables trimming.
    vtkSmartPointer<vtkImageData> m_trimmedImageData;
    bool m_initialCameraReset = false;
};

}
}
