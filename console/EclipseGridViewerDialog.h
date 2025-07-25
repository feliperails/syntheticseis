#pragma once

#include <QDialog>
#include <memory>
#include "geometry/src/Volume.h"

#include <vtkActor.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkSmartPointer.h>

namespace Ui {
    class EclipseGridViewerDialog;
}

using namespace syntheticSeismic;
using namespace syntheticSeismic::geometry;

class EclipseGridViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EclipseGridViewerDialog(const std::vector<std::shared_ptr<Volume>> & allVolumes, QWidget *parent = nullptr);
    ~EclipseGridViewerDialog();

private:
    void buildGrid();


    Ui::EclipseGridViewerDialog *ui;
    const std::vector<std::shared_ptr<Volume>> m_allVolumes;

    vtkSmartPointer<vtkActor> m_actor;
    vtkSmartPointer<vtkTransform> m_transform;
    vtkSmartPointer<vtkTransformFilter> m_transformFilter;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
};
