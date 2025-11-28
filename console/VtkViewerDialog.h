#pragma once

#include <QDialog>

#include <vtkGenericOpenGLRenderWindow.h>

namespace Ui {
    class VtkViewerDialog;
}

namespace syntheticSeismic {
namespace widgets {


class VtkViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VtkViewerDialog(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow, const double& initialZoomFactorZ, QWidget *parent = nullptr);
    ~VtkViewerDialog();

private:
    Ui::VtkViewerDialog *ui;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    double m_bounds[6] = {0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
    bool m_boundsInitialized = false;
};

}
}
