#pragma once

#include <QWizardPage>
#include <QProgressDialog>
#include <QMessageBox>
#include <memory>
#include "domain/src/Lithology.h"

namespace syntheticSeismic {
namespace widgets {

class Worker;
class RegularGridWorker;
class SegyCreationPagePrivate;
class SegyCreationPage : public QWizardPage
{
    Q_OBJECT

signals:
    void progressUpdated(int value);
    void processFinished();

public:
    explicit SegyCreationPage(QWidget* parent);
    virtual ~SegyCreationPage();

    bool isComplete() const override;
    bool validatePage() override;
    void initializePage() override;

    void process();

private:
    bool validateLithologies();
    void initProcessThread();
    std::shared_ptr<domain::Lithology> createFillingLithology(const domain::FillingType& fillingType);

private slots:
    void showProcessMessage();

    void showVisualizerDialog();
    void createDepthVtkRegularGrid();
    void createAmplitudeVtkRegularGrid();

private:
    QThread* m_processThread;
    QProgressDialog* m_progressDialog = nullptr;
    Worker* m_processWorker;
    QPair<QMessageBox::Icon, QString> m_processMessage;

    RegularGridWorker* m_regularGridWorker = nullptr;

    Q_DECLARE_PRIVATE(SegyCreationPage)
    SegyCreationPagePrivate* const d_ptr;
};

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker(SegyCreationPage* segyCreationPage);

signals:
    void progressUpdated(int value);
    void finished();

public slots:
    void run();

private:
    SegyCreationPage * m_segyCreationPage;
};

} // widgets
} // syntheticSeismic
