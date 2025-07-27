#pragma once

#include <QDialog>

#include <vtkGenericOpenGLRenderWindow.h>

namespace Ui {
    class VtkViewerDialog;
}

class VtkViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VtkViewerDialog(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow, QWidget *parent = nullptr);
    ~VtkViewerDialog();

private:
    Ui::VtkViewerDialog *ui;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
};
