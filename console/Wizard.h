#ifndef INC_SYNTHETICSEISMIC_WIDGETS_WIZARD_H
#define INC_SYNTHETICSEISMIC_WIDGETS_WIZARD_H

#include <QWizard>

namespace Ui {
class ProcessingPage;
class RegularGridImportPage;
}

namespace syntheticSeismic {
namespace widgets {

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

class RegularGridImportPagePrivate;
class RegularGridImportPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit RegularGridImportPage(QWidget* parent);

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

private:
    Q_DECLARE_PRIVATE(ProcessingPage)
    ProcessingPagePrivate* const d_ptr;
};

}
}

#endif
