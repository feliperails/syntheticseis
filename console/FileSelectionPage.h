#pragma once

#include <QDialog>
#include <QWizard>
#include <QThread>
#include <QMessageBox>
#include <QProgressDialog>

#include <QSharedData>

namespace syntheticSeismic {
namespace widgets {

class FileSelectionPagePrivate;
class FileSelectionPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit FileSelectionPage(QWidget* parent);

    bool validatePage() override;
    bool isComplete() const override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Q_DECLARE_PRIVATE(FileSelectionPage)
    FileSelectionPagePrivate* const d_ptr;
};

} // widgets
} // syntheticSeismic
