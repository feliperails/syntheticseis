#include "VtkViewerDialog.h"
#include "ui_VtkViewerDialog.h"

#include <vtkRendererCollection.h>


VtkViewerDialog::VtkViewerDialog(vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VtkViewerDialog),
    m_renderWindow(renderWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

    ui->vtkWidget->setRenderWindow(m_renderWindow);

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
}

VtkViewerDialog::~VtkViewerDialog()
{
    delete ui;
}


