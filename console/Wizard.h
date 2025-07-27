#pragma once

#include <QDialog>
#include <QWizard>
#include <QThread>
#include <QMessageBox>
#include <QProgressDialog>

#include <QSharedData>

#include "EclipseGridWorker.h"
#include "domain/src/Lithology.h"


#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>

namespace syntheticSeismic {
namespace widgets {

class PageData: public QSharedData
{

};

/***************************************************************/

class WizardPrivate;
class Wizard: public QWizard
{
    Q_OBJECT
public:
    explicit Wizard(QWidget* parent = nullptr);

private:
    Q_DECLARE_PRIVATE(Wizard)
    WizardPrivate* const d_ptr;
};

class EclipseGridImportPagePrivate;
class EclipseGridImportPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit EclipseGridImportPage(QWidget* parent);

    bool validatePage() override;
    void initializePage() override;
    bool isComplete() const override;

private Q_SLOTS:
    void showVisualizerDialog();
    void createVtkEclipseGrid();

private:
    Q_DECLARE_PRIVATE(EclipseGridImportPage)
    EclipseGridImportPagePrivate* const d_ptr;

    EclipseGridWorker* m_eclipseGridWorker = nullptr;
    QProgressDialog* m_progressDialog = nullptr;

};


class AddingVelocityWidgetPrivate;
class AddingVelocityWidget : public QDialog
{
    Q_OBJECT
private:
    AddingVelocityWidget(const QVector<syntheticSeismic::domain::Lithology>& lithologies, QWidget* parent);

    syntheticSeismic::domain::Lithology lithology() const;

private:
    friend class SegyCreationPagePrivate;
    Q_DECLARE_PRIVATE(AddingVelocityWidget)
    AddingVelocityWidgetPrivate* const d_ptr;
};

}
}
