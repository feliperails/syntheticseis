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

class RegularGridImportPagePrivate;
class RegularGridImportPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit RegularGridImportPage(QWidget* parent);

    bool validatePage() override;
    void initializePage() override;
    bool isComplete() const override;

private:
    Q_DECLARE_PRIVATE(RegularGridImportPage)
    RegularGridImportPagePrivate* const d_ptr;
};

/***************************************************************/

class ProcessingPagePrivate;
class ProcessingPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit ProcessingPage(QWidget* parent);

    bool isComplete() const override;
    bool validatePage() override;
    void initializePage() override;

private:
    Q_DECLARE_PRIVATE(ProcessingPage)
    ProcessingPagePrivate* const d_ptr;
};

class AddingVelocityWidgetPrivate;
class AddingVelocityWidget : public QDialog
{
    Q_OBJECT
private:
    AddingVelocityWidget(const QVector<syntheticSeismic::domain::Lithology>& lithologies, QWidget* parent);

    syntheticSeismic::domain::Lithology lithology() const;

private:
    friend class ProcessingPagePrivate;
    Q_DECLARE_PRIVATE(AddingVelocityWidget)
    AddingVelocityWidgetPrivate* const d_ptr;
};

}
}


#endif
