#pragma once

#include <QDialog>

#include <vtkGenericOpenGLRenderWindow.h>
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
    explicit SeismicVtkViewerDialog(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow, const double& initialZoomFactorZ, QWidget *parent = nullptr);
    ~SeismicVtkViewerDialog();

protected:
    void done(int result) override;
    void showEvent(QShowEvent* event) override;

private:
    Ui::SeismicVtkViewerDialog *ui;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    bool m_initialCameraReset = false;
};

}
}
