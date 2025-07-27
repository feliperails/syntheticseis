#include "Wizard.h"

#include "ui_AddingVelocityWidget.h"
#include "ui_EclipseGridImportPage.h"

#include "FileSelectionPage.h"
#include "VtkViewerDialog.h"

#include <QPushButton>
#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QMessageBox>
#include <QProcess>
#include <QComboBox>
#include <memory>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <array>
#include "domain/src/EclipseGridSurface.h"
#include "domain/src/ExtractVolumes.h"
#include "domain/src/ExtractMinimumRectangle2D.h"
#include "domain/src/ImpedanceRegularGridCalculator.h"
#include "domain/src/Lithology.h"
#include "domain/src/LithologyDictionary.h"
#include "domain/src/ReflectivityRegularGridCalculator.h"
#include "geometry/src/Volume.h"
#include "storage/src/reader/EclipseGridReader.h"
#include "storage/src/writer/GrdSurfaceWriter.h"
#include "domain/src/CellSizeCalculator.h"
#include "domain/src/ExtractVolumes.h"
#include "geometry/src/Point2D.h"
#include "SegyCreationPage.h"


using syntheticSeismic::domain::Lithology;
using namespace syntheticSeismic::storage;
using namespace syntheticSeismic::geometry;

namespace syntheticSeismic {
namespace widgets {

namespace  {
const QLatin1Literal INPUT_FILES_FIELD ("input_files");
}

class WizardPrivate
{
    explicit WizardPrivate(Wizard* q);

    Q_DECLARE_PUBLIC(Wizard)
    Wizard* const q_ptr;
};

    WizardPrivate::WizardPrivate(Wizard *q)
    : q_ptr(q)
{
    FileSelectionPage* fileSelectionPage = new FileSelectionPage(q);
    EclipseGridImportPage* importPage = new EclipseGridImportPage(q);
    SegyCreationPage* segyCreationPage = new SegyCreationPage(q);

    q->addPage(fileSelectionPage);
    q->addPage(importPage);
    q->addPage(segyCreationPage);

    q->button(QWizard::NextButton)->setEnabled(false);
}

Wizard::Wizard(QWidget* parent)
    : QWizard(parent)
    , d_ptr(new WizardPrivate(this))
{
}

/************************************************************************************************************************************************************************/

namespace {
const int MESSAGE_COLUMN = 1;
const int DIMENSIONS_COLUMN = 2;
const int EXPORT_MAIN_SURFACE_COLUMN = 3;
const int EXPORT_SURFACE_BY_AGE_COLUMN = 4;
const QString ECLIPSE_FILE_IMPORTED = QObject::tr("Imported");


const QString REGULAR_GRID_X_DIMENSION = QLatin1String("regularGridXDimension");
const QString REGULAR_GRID_Y_DIMENSION = QLatin1String("regularGridYDimension");
const QString REGULAR_GRID_Z_DIMENSION = QLatin1String("regularGridZDimension");

const QString CELL_SIZE_IN_X = QLatin1String("cellSizeInX");
const QString CELL_SIZE_IN_Y = QLatin1String("cellSizeInY");
const QString CELL_SIZE_IN_Z = QLatin1String("cellSizeInZ");
const QString RICKER_WAVELET_FREQUENCY = QLatin1String("rickerWaveletFrequency");

const QString ECLIPSE_GRIDS = QLatin1String("eclipseGrids");
const QString ALL_VOLUMES = QLatin1String("allVolumes");
const QString MINIMUM_RECTANGLE = QLatin1String("minimumRectangle");

}

using syntheticSeismic::geometry::Volume;
using syntheticSeismic::domain::EclipseGrid;

class EclipseGridImportPagePrivate
{
    explicit EclipseGridImportPagePrivate(EclipseGridImportPage* q);
    void updateWidget();
    void updateCellSize();
    void importEclipseFiles();
    void importEclipseFile(const QPair<QFileInfo, int>&);

    Q_DECLARE_PUBLIC(EclipseGridImportPage)
    EclipseGridImportPage* q_ptr;
    std::unique_ptr<Ui::EclipseGridImportPage> m_ui;
    QHash<QString, QVector<QString> > m_eclipseGridsErros;
    QHash<QString, QVector<QPair<QString, std::shared_ptr<EclipseGrid>>> > m_hashEclipseGrids;
    QVector<std::shared_ptr<EclipseGrid>> m_eclipseGrids;
    std::vector<std::shared_ptr<Volume>> m_allVolumes;
    std::array<Point2D, 4> m_minimumRectangle;
    std::shared_ptr<domain::CellSizeCalculator> m_cellSizeCalculator;
    QMutex m_mutexEclipseGrids;
    bool m_eclipseFilesImported;
};

EclipseGridImportPagePrivate::EclipseGridImportPagePrivate(EclipseGridImportPage *q)
    : q_ptr(q)
    , m_ui(std::make_unique<Ui::EclipseGridImportPage>())
    , m_hashEclipseGrids()
    , m_eclipseFilesImported(false)
{
    m_ui->setupUi(q);

    const auto emitCompleteChangedFunction = [this](const double) -> void {
        Q_EMIT q_ptr->completeChanged();
    };



    QObject::connect(m_ui->rickerWaveletFrequencyDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, emitCompleteChangedFunction);
    QObject::connect(m_ui->regularGridXDimensionDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, emitCompleteChangedFunction);
    QObject::connect(m_ui->regularGridYDimensionDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, emitCompleteChangedFunction);
    QObject::connect(m_ui->regularGridZDimensionDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, emitCompleteChangedFunction);

    const auto updateCellSizeFunction = [this](const double) -> void {
        this->updateCellSize();
    };

    QObject::connect(m_ui->rickerWaveletFrequencyDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, updateCellSizeFunction);
    QObject::connect(m_ui->regularGridXDimensionDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, updateCellSizeFunction);
    QObject::connect(m_ui->regularGridYDimensionDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, updateCellSizeFunction);
    QObject::connect(m_ui->regularGridZDimensionDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, updateCellSizeFunction);
}

void EclipseGridImportPagePrivate::updateCellSize()
{
    if (m_minimumRectangle.empty()) return;


    if (m_cellSizeCalculator.get() == nullptr)
    {
        m_cellSizeCalculator = std::make_shared<domain::CellSizeCalculator>(
                m_ui->regularGridXDimensionDoubleSpinBox->value(),
                m_ui->regularGridYDimensionDoubleSpinBox->value(),
                m_ui->regularGridZDimensionDoubleSpinBox->value(),
                m_ui->rickerWaveletFrequencyDoubleSpinBox->value(),
                m_minimumRectangle
            );
    }

    m_cellSizeCalculator->setNumberOfCellsInX(m_ui->regularGridXDimensionDoubleSpinBox->value());
    m_cellSizeCalculator->setNumberOfCellsInY(m_ui->regularGridYDimensionDoubleSpinBox->value());
    m_cellSizeCalculator->setNumberOfCellsInZ(m_ui->regularGridZDimensionDoubleSpinBox->value());
    m_cellSizeCalculator->setRickerWaveletFrequency(m_ui->rickerWaveletFrequencyDoubleSpinBox->value());


    m_cellSizeCalculator->calculate();

    m_ui->regularGridXCellSizeDoubleSpinBox->setValue(m_cellSizeCalculator->getCellSizeInX());
    m_ui->regularGridYCellSizeDoubleSpinBox->setValue(m_cellSizeCalculator->getCellSizeInY());
    m_ui->regularGridZCellSizeDoubleSpinBox->setValue(m_cellSizeCalculator->getCellSizeInZ());
}

void EclipseGridImportPagePrivate::updateWidget()
{
    using namespace syntheticSeismic::domain;

    importEclipseFiles();

    Q_Q(EclipseGridImportPage);

    m_ui->fileTreeWidget->clear();
    m_eclipseGrids.clear();
    m_allVolumes.clear();

    QList<QString> dirs = m_hashEclipseGrids.keys();
    std::sort(dirs.begin(), dirs.end());

    for (int i = 0, countI = dirs.size(); i < countI; ++i) {

        const QString& dir = dirs.at(i);
        QTreeWidgetItem* topLevelItem = new QTreeWidgetItem({dir});

        m_ui->fileTreeWidget->addTopLevelItem(topLevelItem);

        const QVector<QPair<QString, std::shared_ptr<EclipseGrid>>> baseFileNames = m_hashEclipseGrids.value(dir);

        for (int j = 0, countJ = baseFileNames.size(); j < countJ; ++j) {
            const QPair<QString, std::shared_ptr<EclipseGrid>>& pair = baseFileNames.at(j);

            const auto eclipsGrid = pair.second;
            Q_ASSERT(eclipsGrid);
            if (eclipsGrid) {
                m_eclipseGrids.push_back(eclipsGrid);
                const std::vector<std::shared_ptr<Volume>> volumesOfFirstLayer = ExtractVolumes::extractFirstLayerFrom(*eclipsGrid);
                const std::vector<std::shared_ptr<Volume>> volumes = ExtractVolumes::extractFromVolumesOfFirstLayer(volumesOfFirstLayer, *eclipsGrid, true);
                m_allVolumes.insert(m_allVolumes.end(), volumes.begin(), volumes.end());
            }

            const QString& fileName = pair.first;

            QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem({fileName});
            topLevelItem->addChild(treeWidgetItem);

            const QString& error = m_eclipseGridsErros[dir][j];
            const std::shared_ptr<EclipseGrid>& eclipseGrid = m_hashEclipseGrids[dir][j].second;
            if (error.isEmpty()) {
                treeWidgetItem->setText(MESSAGE_COLUMN, ECLIPSE_FILE_IMPORTED);

                const QString text = QLatin1String("X: ") + QString::number(eclipseGrid->numberOfCellsInX()) +
                        QLatin1String(", Y: ") + QString::number(eclipseGrid->numberOfCellsInY()) +
                        QLatin1String(", Z: ") + QString::number(eclipseGrid->numberOfCellsInZ());

                treeWidgetItem->setText(DIMENSIONS_COLUMN, text);
            } else {
                treeWidgetItem->setText(MESSAGE_COLUMN, error);
                treeWidgetItem->setText(DIMENSIONS_COLUMN, QLatin1Literal("--"));
                m_eclipseFilesImported = false;
            }

            const QIcon exportIcon = QIcon(QLatin1String(":/export"));
            const QPixmap exportPixmap(QLatin1String(":/export"));

            // BEGIN EXPORT MAIN SURFACE
            QPushButton* exportSurfaceAction = new QPushButton(q);
            exportSurfaceAction->setIcon(exportIcon);
            exportSurfaceAction->setFixedWidth(exportPixmap.width());

            QObject::connect(exportSurfaceAction, &QPushButton::clicked, q, [eclipseGrid](const bool) {
                using syntheticSeismic::domain::EclipseGridSurface;
                using syntheticSeismic::storage::GrdSurfaceWriter;

                const auto activeWindow = QApplication::activeWindow();
                const auto title = QObject::tr("Number of surface cells");
                const auto dimX = QInputDialog::getInt(activeWindow, title, QObject::tr("Number of cells in x-axis:"), eclipseGrid->numberOfCellsInX());
                const auto dimY = QInputDialog::getInt(activeWindow, title, QObject::tr("Number of cells in y-axis:"), eclipseGrid->numberOfCellsInY());

                const auto path = QFileDialog::getSaveFileName(QApplication::activeWindow(), QObject::tr("Export main surface"), "", QObject::tr("GRD(*.grd)"));

                if (!path.isEmpty())
                {
                    EclipseGridSurface eclipseGridSurface(eclipseGrid, dimX, dimY);
                    auto result = eclipseGridSurface.extractMainSurface();

                    GrdSurfaceWriter<double> grdSurfaceWriter(path);
                    grdSurfaceWriter.write(*result->getSurface());

                    const auto lithologyPath = path.mid(0, path.length() - 4) + "_lithology.grd";
                    GrdSurfaceWriter<int> grdLithologySurfaceWriter(lithologyPath);
                    grdLithologySurfaceWriter.write(*result->getLithologySurface());

                    const auto faciesAssociationPath = path.mid(0, path.length() - 4) + "_facies_association.grd";
                    GrdSurfaceWriter<int> grdFaciesAssociationSurfaceWriter(faciesAssociationPath);
                    grdFaciesAssociationSurfaceWriter.write(*result->getFaciesAssociationSurface());
                }
            });
            // END EXPORT MAIN SURFACE

            m_minimumRectangle = ExtractMinimumRectangle2D::extractFrom(m_allVolumes);

            std::cout << std::endl << std::endl << "minimumRectangle 0 EclipseGridImportPagePrivate::updateWidget(): " << std::endl << std::endl;
            foreach (auto var, m_minimumRectangle) {
                std::cout << "var.x: " << var.x << " -- var.y: " << var.y << std::endl;

            }


            m_ui->regularGridXDimensionDoubleSpinBox->setValue(eclipseGrid->numberOfCellsInX());
            m_ui->regularGridYDimensionDoubleSpinBox->setValue(eclipseGrid->numberOfCellsInY());
            m_ui->regularGridZDimensionDoubleSpinBox->setValue(eclipseGrid->numberOfCellsInZ());
            m_ui->rickerWaveletFrequencyDoubleSpinBox->setValue(1.0);
            m_ui->fileTreeWidget->setItemWidget(treeWidgetItem, EXPORT_MAIN_SURFACE_COLUMN, exportSurfaceAction);

            updateCellSize();

            // BEGIN EXPORT SURFACE BY AGE
            QPushButton* exportSurfaceByAgeAction = new QPushButton(q);
            exportSurfaceByAgeAction->setIcon(exportIcon);
            exportSurfaceByAgeAction->setFixedWidth(exportPixmap.width());

            QObject::connect(exportSurfaceByAgeAction, &QPushButton::clicked, q, [eclipseGrid](const bool) {
                using syntheticSeismic::domain::EclipseGridSurface;
                using syntheticSeismic::storage::GrdSurfaceWriter;

                const auto activeWindow = QApplication::activeWindow();
                const auto title = QObject::tr("Number of surface cells");

                const auto dimX = QInputDialog::getInt(activeWindow,
                                                       title,
                                                       QObject::tr("Number of cells in x-axis:"),
                                                       eclipseGrid->numberOfCellsInX()

                                                       );
                const auto dimY = QInputDialog::getInt(activeWindow,
                                                       title,
                                                       QObject::tr("Number of cells in y-axis:"),
                                                       eclipseGrid->numberOfCellsInY()
                                                    );

                const auto rotationSupport = QMessageBox::question(activeWindow,
                                              title,
                                              "Rotation support?",
                                              QMessageBox::Yes|QMessageBox::No
                                            );

                const auto ageMultiplicationFactorToIdentifyAges = QInputDialog::getInt(
                            activeWindow,
                            title,
                            QObject::tr("Age multiplication factor to identify ages:"),
                            6
                        );

                const QString path = QFileDialog::getExistingDirectory(
                            QApplication::activeWindow(),
                            QObject::tr("Export surfaces by age"),
                            "",
                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                        );

                if (!path.isEmpty())
                {
                    EclipseGridSurface eclipseGridSurface(eclipseGrid, dimX, dimY);
                    auto results = eclipseGridSurface.extractSurfacesByAge(
                                rotationSupport == QMessageBox::Yes,
                                ageMultiplicationFactorToIdentifyAges
                            );

                    #pragma omp parallel for
                    for (int iAux = 0; iAux < results.size(); ++iAux)
                    {
                        const auto i = static_cast<size_t>(iAux);
                        const auto& result = results[i];
                        const auto basePath = path + "/" + QString::number(result->getAge(), 'f', ageMultiplicationFactorToIdentifyAges);

                        const auto surfacePath = basePath + ".grd";
                        GrdSurfaceWriter<double> grdSurfaceWriter(surfacePath);
                        grdSurfaceWriter.write(*result->getSurface());

                        const auto lithologyPath = basePath + "_lithology.grd";
                        GrdSurfaceWriter<int> grdLithologySurfaceWriter(lithologyPath);
                        grdLithologySurfaceWriter.write(*result->getLithologySurface());

                        const auto faciesAssociationPath = basePath + "_faciesAssociation.grd";
                        GrdSurfaceWriter<int> grdFaciesAssociationSurfaceWriter(faciesAssociationPath);
                        grdFaciesAssociationSurfaceWriter.write(*result->getFaciesAssociationSurface());

                        const auto agePath = basePath + "_age.grd";
                        GrdSurfaceWriter<double> grdAgeSurfaceWriter(agePath);
                        grdAgeSurfaceWriter.write(*result->getAgeSurface());
                    }
                }
            });

            m_ui->fileTreeWidget->setItemWidget(treeWidgetItem, EXPORT_SURFACE_BY_AGE_COLUMN, exportSurfaceByAgeAction);
            // END EXPORT SURFACE BY AGE
        }


    }

    m_ui->fileTreeWidget->expandAll();
    m_ui->fileTreeWidget->resizeColumnToContents(0);
    Q_EMIT q->completeChanged();
}

void EclipseGridImportPagePrivate::importEclipseFile(const QPair<QFileInfo, int>& fileInfoAndIndex)
{
    const QFileInfo& fileInfo = fileInfoAndIndex.first;
    const int& j = fileInfoAndIndex.second;
    const QString& dir = fileInfo.dir().path();

    QString error;
    EclipseGridReader reader(fileInfo.filePath());
    const std::shared_ptr<EclipseGrid>& eclipseGrid = std::make_shared<EclipseGrid>(reader.read(error));

    m_mutexEclipseGrids.lock();

        m_hashEclipseGrids[dir][j].second = eclipseGrid;

        if (!error.isEmpty())
        {
            m_eclipseGridsErros[dir][j] = error;
            m_eclipseFilesImported = false;
        }

    m_mutexEclipseGrids.unlock();
}

void EclipseGridImportPagePrivate::importEclipseFiles()
{
    QList<QString> dirs = m_hashEclipseGrids.keys();
    std::sort(dirs.begin(), dirs.end());
    m_eclipseFilesImported = true;

    QVector<QPair<QFileInfo, int>> fileInfosForImport;

    for (int i = 0, countI = dirs.size(); i < countI; ++i) {

        const QString& dir = dirs.at(i);

        const QVector<QPair<QString, std::shared_ptr<EclipseGrid>>> baseFileNames = m_hashEclipseGrids.value(dir);

        for (int j = 0, countJ = baseFileNames.size(); j < countJ; ++j) {
            const QString& fileName = baseFileNames.at(j).first;

            QFileInfo fileInfo;
            fileInfo.setFile(dir, fileName);

            fileInfosForImport.push_back(QPair<QFileInfo, int>(fileInfo, j));
        }
    }

    QProgressDialog dialog("Importing...", nullptr, 0, 100);
    dialog.setWindowTitle("Importing Files");
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowCloseButtonHint);

    QFutureWatcher<void> futureWatcher;
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
    QObject::connect(&dialog, &QProgressDialog::canceled, &futureWatcher, &QFutureWatcher<void>::cancel);
    QObject::connect(&futureWatcher,  &QFutureWatcher<void>::progressRangeChanged, &dialog, &QProgressDialog::setRange);
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged,  &dialog, &QProgressDialog::setValue);

    futureWatcher.setFuture(QtConcurrent::map(fileInfosForImport,
                                              [this](const QPair<QFileInfo, int>& fileInfoAndIndex) {
                                                  importEclipseFile(fileInfoAndIndex);
                                              }));

    dialog.exec();
    futureWatcher.waitForFinished();
}

/*------------------------------------------------------------------------------------------------------------------------*/

EclipseGridImportPage::EclipseGridImportPage(QWidget* parent)
    : QWizardPage(parent),
      d_ptr(new EclipseGridImportPagePrivate(this))
{
    registerField(REGULAR_GRID_X_DIMENSION, d_ptr->m_ui->regularGridXDimensionDoubleSpinBox);
    registerField(REGULAR_GRID_Y_DIMENSION, d_ptr->m_ui->regularGridYDimensionDoubleSpinBox);
    registerField(REGULAR_GRID_Z_DIMENSION, d_ptr->m_ui->regularGridZDimensionDoubleSpinBox);


    registerField(CELL_SIZE_IN_X, d_ptr->m_ui->regularGridXCellSizeDoubleSpinBox);
    registerField(CELL_SIZE_IN_Y, d_ptr->m_ui->regularGridYCellSizeDoubleSpinBox);
    registerField(CELL_SIZE_IN_Z, d_ptr->m_ui->regularGridZCellSizeDoubleSpinBox);
    registerField(RICKER_WAVELET_FREQUENCY, d_ptr->m_ui->rickerWaveletFrequencyDoubleSpinBox);

    registerField(ECLIPSE_GRIDS, d_ptr->m_ui->fileTreeWidget);

    auto dummyWidgetAllVolumes = new QWidget(this);
    auto dummyWidgetMinRectangle = new QWidget(this);

    dummyWidgetAllVolumes->hide();
    dummyWidgetMinRectangle->hide();

    registerField(ALL_VOLUMES, dummyWidgetAllVolumes);
    registerField(MINIMUM_RECTANGLE, dummyWidgetMinRectangle);

    connect(d_ptr->m_ui->visualizeToolButton, &QToolButton::clicked,
                    this, &EclipseGridImportPage::createVtkEclipseGrid);
}

void EclipseGridImportPage::createVtkEclipseGrid()
{
    d_ptr->m_ui->visualizeToolButton->setEnabled(false);

    auto workerThread = new QThread(this);

    m_eclipseGridWorker = new EclipseGridWorker(&d_ptr->m_allVolumes);

    m_eclipseGridWorker->moveToThread(workerThread);

    m_progressDialog = new QProgressDialog("Processing...", nullptr, 0, 100, this);
    m_progressDialog->setWindowTitle("Building Grid");
    m_progressDialog->setWindowFlags(m_progressDialog->windowFlags() & ~Qt::WindowCloseButtonHint);


    m_progressDialog->show();

    connect(workerThread, &QThread::started, m_eclipseGridWorker, &EclipseGridWorker::run);
    connect(m_eclipseGridWorker, &EclipseGridWorker::stepProgress, m_progressDialog, &QProgressDialog::setValue);
    connect(m_eclipseGridWorker, &EclipseGridWorker::finished, this, &EclipseGridImportPage::showVisualizerDialog);
    connect(m_eclipseGridWorker, &EclipseGridWorker::finished, workerThread, &QThread::quit);
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);

    workerThread->start();
}

void EclipseGridImportPage::showVisualizerDialog()
{
    m_progressDialog->close();
    delete m_progressDialog;
    m_progressDialog = nullptr;

    d_ptr->m_ui->visualizeToolButton->setEnabled(true);


    VtkViewerDialog dialog(m_eclipseGridWorker->getRenderWindow());

    dialog.exec();

    delete m_eclipseGridWorker;
    m_eclipseGridWorker = nullptr;
}

bool EclipseGridImportPage::validatePage()
{
    setField(REGULAR_GRID_X_DIMENSION, d_ptr->m_ui->regularGridXDimensionDoubleSpinBox->value());
    setField(REGULAR_GRID_Y_DIMENSION, d_ptr->m_ui->regularGridYDimensionDoubleSpinBox->value());
    setField(REGULAR_GRID_Z_DIMENSION, d_ptr->m_ui->regularGridZDimensionDoubleSpinBox->value());

    setField(CELL_SIZE_IN_X, d_ptr->m_ui->regularGridXCellSizeDoubleSpinBox->value());
    setField(CELL_SIZE_IN_Y, d_ptr->m_ui->regularGridYCellSizeDoubleSpinBox->value());
    setField(CELL_SIZE_IN_Z, d_ptr->m_ui->regularGridZCellSizeDoubleSpinBox->value());
    setField(RICKER_WAVELET_FREQUENCY, d_ptr->m_ui->rickerWaveletFrequencyDoubleSpinBox->value());

    setField(ECLIPSE_GRIDS, QVariant::fromValue(d_ptr->m_eclipseGrids));
    setField(ALL_VOLUMES, QVariant::fromValue(d_ptr->m_allVolumes));
    setField(MINIMUM_RECTANGLE, QVariant::fromValue(d_ptr->m_minimumRectangle));

    return true;
}

void EclipseGridImportPage::initializePage()
{
    Q_D(EclipseGridImportPage);
    d_ptr->m_allVolumes.clear();
    d_ptr->m_minimumRectangle = std::array<Point2D, 4>();
    d_ptr->m_eclipseGrids.clear();
    d->m_hashEclipseGrids.clear();
    d->m_eclipseGridsErros.clear();
    const QHash<QString, QSet<QString>> fileNames = field(INPUT_FILES_FIELD).value<QHash<QString, QSet<QString>>>();

    const QList<QString> dirs = fileNames.keys();
    for (const QString& dir : dirs) {
        const QSet<QString> files = fileNames.value(dir);
        for (const QString& file : files) {
            d->m_eclipseGridsErros[dir].push_back("");
            d->m_hashEclipseGrids[dir].push_back(QPair<QString, std::shared_ptr<syntheticSeismic::domain::EclipseGrid>>(file, std::shared_ptr<syntheticSeismic::domain::EclipseGrid>()));
        }
    }
    d->updateWidget();
}

bool EclipseGridImportPage::isComplete() const
{
    Q_D(const EclipseGridImportPage);
    return d->m_ui->fileTreeWidget->topLevelItemCount() != 0
            && d->m_eclipseFilesImported
            && !qFuzzyIsNull(d->m_ui->rickerWaveletFrequencyDoubleSpinBox->value())
            && !qFuzzyIsNull(d->m_ui->regularGridXDimensionDoubleSpinBox->value())
            && !qFuzzyIsNull(d->m_ui->regularGridYDimensionDoubleSpinBox->value())
            && !qFuzzyIsNull(d->m_ui->regularGridZDimensionDoubleSpinBox->value());
}

class AddingVelocityWidgetPrivate
{
    explicit AddingVelocityWidgetPrivate(AddingVelocityWidget* q, const QVector<domain::Lithology> &lithologies);
    bool defineState();

    Q_DECLARE_PUBLIC(AddingVelocityWidget)
    AddingVelocityWidget* q_ptr;
    std::unique_ptr<Ui::AddingVelocityWidget> m_ui;
    const QVector<syntheticSeismic::domain::Lithology>& m_lithologies;
};

AddingVelocityWidgetPrivate::AddingVelocityWidgetPrivate(AddingVelocityWidget *q, const QVector<domain::Lithology> &lithologies)
    : q_ptr(q)
    , m_ui(std::make_unique<Ui::AddingVelocityWidget>())
    , m_lithologies(lithologies)
{
    m_ui->setupUi(q);

    int identifier = 0;
    for (const Lithology& lithology : m_lithologies) {
        identifier = std::max(identifier, lithology.getId());
    }
    ++identifier;
    m_ui->identifierSpinBox->setValue(identifier);

    QObject::connect(m_ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, q_ptr, [this](const bool){
        q_ptr->accept();
    });

    QObject::connect(m_ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, q_ptr, [this](const bool){
        q_ptr->reject();
    });

    QObject::connect(m_ui->densityDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, [this](const double){
        defineState();
    });

    QObject::connect(m_ui->velocityDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, [this](const double){
        defineState();
    });

    QObject::connect(m_ui->identifierSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), q_ptr, [this](const double){
        defineState();
    });

    defineState();
}

bool AddingVelocityWidgetPrivate::defineState()
{
    QString msg;
    m_ui->infoLabel->clear();

    QPushButton* okButton = m_ui->buttonBox->button(QDialogButtonBox::Ok);
    Q_ASSERT(okButton);

    if (m_ui->nameLineEdit->text().isEmpty()) {
        okButton->setEnabled(false);
    } else {
        const int identifier = m_ui->identifierSpinBox->value();
        const QString name = m_ui->nameLineEdit->text();
        for (const Lithology& lithology : m_lithologies) {
            if (lithology.getId() == identifier) {
                msg = QObject::tr("The velocity identifier must be unique.\n");
            } else {
                if (lithology.getName() == name) {
                    msg += QObject::tr("The velocity name must be unique.\n");
                }
            }
        }
    }

    m_ui->infoLabel->setText(msg);
    okButton->setEnabled(msg.isEmpty());

    return okButton->isEnabled();
}


/*-------------------------------------------------------------------------------------------------------------------------*/

AddingVelocityWidget::AddingVelocityWidget(const QVector<domain::Lithology> &lithologies, QWidget *parent)
    : QDialog(parent)
    , d_ptr(new AddingVelocityWidgetPrivate(this, lithologies))
{
}

Lithology AddingVelocityWidget::lithology() const
{
    Q_D(const AddingVelocityWidget);
    return Lithology(d->m_ui->identifierSpinBox->value(),
                     d->m_ui->nameLineEdit->text(),
                     d->m_ui->velocityDoubleSpinBox->value(),
                     d->m_ui->densityDoubleSpinBox->value());
}

}
}

Q_DECLARE_METATYPE(syntheticSeismic::geometry::Volume)
Q_DECLARE_METATYPE(std::shared_ptr<syntheticSeismic::geometry::Volume>)
Q_DECLARE_METATYPE(std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>>)
Q_DECLARE_METATYPE(Point2D)
Q_DECLARE_METATYPE(MinRectangle2D)
