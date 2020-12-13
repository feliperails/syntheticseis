#ifndef INC_SYNTHETICSEISMIC_WIDGETS_WIZARD_H
#define INC_SYNTHETICSEISMIC_WIDGETS_WIZARD_H

#include <QDialog>
#include <QWizard>

#include <QSharedData>

#include "domain/src/Lithology.h"

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

/***************************************************************/

class FileSelectionPagePrivate;
class FileSelectionPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit FileSelectionPage(QWidget* parent);

    bool validatePage() override;
    bool isComplete() const override;

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Q_DECLARE_PRIVATE(FileSelectionPage)
    FileSelectionPagePrivate* const d_ptr;
};

/***************************************************************/

class EclipseGridImportPagePrivate;
class EclipseGridImportPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit EclipseGridImportPage(QWidget* parent);

    bool validatePage() override;
    void initializePage() override;
    bool isComplete() const override;

private:
    Q_DECLARE_PRIVATE(EclipseGridImportPage)
    EclipseGridImportPagePrivate* const d_ptr;
};

/***************************************************************/

class SegyCreationPagePrivate;
class SegyCreationPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SegyCreationPage(QWidget* parent);

    bool isComplete() const override;
    bool validatePage() override;
    void initializePage() override;

private:
    Q_DECLARE_PRIVATE(SegyCreationPage)
    SegyCreationPagePrivate* const d_ptr;
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


#endif
