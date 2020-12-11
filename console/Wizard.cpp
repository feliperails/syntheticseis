#include "Wizard.h"

#include "ui_ProcessingPage.h"
#include "ui_RegularGridImportPage.h"

#include <memory>

namespace syntheticSeismic {
namespace widgets {

class WizardPrivate
{
    explicit WizardPrivate(Wizard* q);

    Q_DECLARE_PUBLIC(Wizard)
    Wizard* const q_ptr;
};

WizardPrivate::WizardPrivate(Wizard *q)
    : q_ptr(q)
{
//    const QString transparentBackground = QStringLiteral("background-color: rgba(0,0,0,0)");

    RegularGridImportPage* firstPage = new RegularGridImportPage(q);
//    firstPage->setStyleSheet(transparentBackground);

    ProcessingPage* secondPage = new ProcessingPage(q);
//    secondPage->setStyleSheet(transparentBackground);

    q->addPage(firstPage);
    q->addPage(secondPage);
}

Wizard::Wizard(QWidget* parent)
    : QWizard(parent)
    , d_ptr(new WizardPrivate(this))
{
}

/************************************************************************************************************************************************************************/

class RegularGridImportPagePrivate
{
    explicit RegularGridImportPagePrivate(RegularGridImportPage* q);

    Q_DECLARE_PUBLIC(RegularGridImportPage)
    RegularGridImportPage* q_ptr;
    std::unique_ptr<Ui::RegularGridImportPage> m_ui;
};

RegularGridImportPagePrivate::RegularGridImportPagePrivate(RegularGridImportPage *q)
    : q_ptr(q)
    , m_ui(std::make_unique<Ui::RegularGridImportPage>())
{
    m_ui->setupUi(q);
}

RegularGridImportPage::RegularGridImportPage(QWidget* parent)
    : QWizardPage(parent),
      d_ptr(new RegularGridImportPagePrivate(this))
{
}

/************************************************************************************************************************************************************************/

class ProcessingPagePrivate
{
    explicit ProcessingPagePrivate(ProcessingPage* q);

    Q_DECLARE_PUBLIC(ProcessingPage)
    ProcessingPage* q_ptr;
    std::unique_ptr<Ui::ProcessingPage> m_ui;
};

ProcessingPagePrivate::ProcessingPagePrivate(ProcessingPage *q)
    : q_ptr(q)
    , m_ui(std::make_unique<Ui::ProcessingPage>())
{
    m_ui->setupUi(q);    
}

ProcessingPage::ProcessingPage(QWidget* parent)
    : QWizardPage(parent),
      d_ptr(new ProcessingPagePrivate(this))
{
}

}
}
