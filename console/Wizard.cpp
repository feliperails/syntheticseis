#include "Wizard.h"

#include "ui_AddingVelocityWidget.h"
#include "ui_FileSelectionPage.h"
#include "ui_SegyCreationPage.h"
#include "ui_EclipseGridImportPage.h"

#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QMessageBox>
#include <QProcess>
#include <memory>
#include "domain/src/ConvolutionRegularGridCalculator.h"
#include "domain/src/EclipseGridSurface.h"
#include "domain/src/ExtractVolumes.h"
#include "domain/src/ExtractMinimumRectangle2D.h"
#include "domain/src/Facade.h"
#include "domain/src/ImpedanceRegularGridCalculator.h"
#include "domain/src/Lithology.h"
#include "domain/src/LithologyDictionary.h"
#include "domain/src/RegularGrid.h"
#include "domain/src/ReflectivityRegularGridCalculator.h"
#include "domain/src/RickerWaveletCalculator.h"
#include "domain/src/RotateVolumeCoordinate.h"
#include "domain/src/VolumeToRegularGrid.h"
#include "geometry/src/Volume.h"
#include "storage/src/RegularGridHdf5Storage.h"
#include "storage/src/reader/EclipseGridReader.h"
#include "storage/src/writer/GrdSurfaceWriter.h"
#include "storage/src/writer/SEGYWriter.h"
#include "domain/src/ConvertRegularGridCalculator.h"

using namespace syntheticSeismic::storage;

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

const int FILE_COLUMN = 0;
const int ACTION_COLUMN = 1;

const QString REMOVE_ACTION_MESSAGE = QObject::tr("Remove");
}

class FileSelectionPagePrivate
{
    explicit FileSelectionPagePrivate(FileSelectionPage* q);
    void updateWidget();
    void removeItem(QTreeWidgetItem *item);
    int calculateActionMessageWidth () const;
    void resizeColumns();

    Q_DECLARE_PUBLIC(FileSelectionPage)
    FileSelectionPage* q_ptr;
    std::unique_ptr<Ui::FileSelectionPage> m_ui;
    QHash<QString, QSet<QString>> m_fileNames;
};

FileSelectionPagePrivate::FileSelectionPagePrivate(FileSelectionPage *q)
    : q_ptr(q)
    , m_ui(std::make_unique<Ui::FileSelectionPage>())
    , m_fileNames()
{
    m_ui->setupUi(q);

    QObject::connect(m_ui->eclipseGridFilePushButton, &QPushButton::clicked, q_ptr, [this](const bool){
        const QStringList fileNames = QFileDialog::getOpenFileNames(q_ptr,
                                                                    QObject::tr("SyntheticSeismic - File seleciton"),
                                                                    QObject::tr("Select EclipseGrid files"),
                                                                    QLatin1String("EclipseGrid (*.grdecl)"));
        QFileInfo fileInfo;
        for (const QString& fileName : fileNames) {
            fileInfo.setFile(fileName);
            const QString dir = fileInfo.dir().path();
            m_fileNames[dir].insert(fileInfo.fileName());
        }

        updateWidget();
    });

    updateWidget();

    QObject::connect(m_ui->cleanSelectedFilesPushButton, &QPushButton::clicked, q_ptr, [this](const bool){
        m_fileNames.clear();
        updateWidget();
    });
}

int FileSelectionPagePrivate::calculateActionMessageWidth () const
{
    Q_Q(const FileSelectionPage);
    return QFontMetrics(q->font()).width(REMOVE_ACTION_MESSAGE);
}

void FileSelectionPagePrivate::updateWidget()
{
    Q_Q(FileSelectionPage);

    m_ui->cleanSelectedFilesPushButton->setEnabled(!m_fileNames.isEmpty());

    for (int i = 0, countI = m_ui->fileTreeWidget->topLevelItemCount(); i < countI; ++i) {
        QTreeWidgetItem* topLevelItem = m_ui->fileTreeWidget->topLevelItem(i);
        if (!topLevelItem) {
            continue;
        }

        for (int j = 0, countJ = topLevelItem->childCount(); j < countJ; ++j) {
            QTreeWidgetItem* item = topLevelItem->child(j);
            if (!item) {
                continue;
            }

            QWidget* widget = m_ui->fileTreeWidget->itemWidget(item, ACTION_COLUMN);
            if (widget) {
                widget->deleteLater();
            }
        }
    }

    m_ui->fileTreeWidget->clear();
    int identifier = 0;

    QList<QString> dirs = m_fileNames.keys();
    std::sort(dirs.begin(), dirs.end());

    const QIcon removeIcon = QIcon(QLatin1String(":/remove"));
    const QPixmap pixmap(QLatin1String(":/remove"));

    for (int i = 0, countI = dirs.size(); i < countI; ++i) {

        const QString& dir = dirs.at(i);
        QTreeWidgetItem* topLevelItem = new QTreeWidgetItem({dir});

        m_ui->fileTreeWidget->addTopLevelItem(topLevelItem);

        QList<QString> baseFileNames = m_fileNames.value(dir).toList();
        std::sort(baseFileNames.begin(), baseFileNames.end());

        for (int j = 0, countJ = baseFileNames.size(); j < countJ; ++j) {
            const QString& fileName = baseFileNames.at(j);

            QTreeWidgetItem* item = new QTreeWidgetItem({fileName, QString()});
            topLevelItem->addChild(item);
            item->setData(ACTION_COLUMN, Qt::UserRole, identifier);

            QPushButton* removeAction = new QPushButton(q);
            removeAction->setIcon(removeIcon);
            removeAction->setFixedWidth(pixmap.width());

            QObject::connect(removeAction, &QPushButton::clicked, q, [this, item](const bool){
                this->removeItem(item);
            });

            m_ui->fileTreeWidget->setItemWidget(item, ACTION_COLUMN, removeAction);
        }
    }

    m_ui->fileTreeWidget->expandAll();
    resizeColumns();
    if (q->isVisible()) {
        Q_EMIT q->completeChanged();
    }
}

void FileSelectionPagePrivate::resizeColumns()
{
    Q_Q(FileSelectionPage);
    const int width = m_ui->fileTreeWidget->width();
    const QPixmap pixmap(QLatin1String(":/remove"));
    m_ui->fileTreeWidget->setColumnWidth(0, width - pixmap.width() *2);
}

void FileSelectionPagePrivate::removeItem(QTreeWidgetItem* item)
{
    Q_Q(FileSelectionPage);

    if (!item) {
        return;
    }

    QWidget* actionWidget = m_ui->fileTreeWidget->itemWidget(item, ACTION_COLUMN);
    if (!actionWidget) {
        return;
    }

    QTreeWidgetItem* parent = item->parent();
    actionWidget->deleteLater();
    delete item;

    if (parent->childCount() == 0) {
        delete parent;
    }

    m_ui->cleanSelectedFilesPushButton->setEnabled(m_ui->fileTreeWidget->topLevelItemCount() != 0);
    Q_EMIT q->completeChanged();
}

/*-----------------------------------------------------------------------------------------------------------------------*/

FileSelectionPage::FileSelectionPage(QWidget* parent)
    : QWizardPage(parent),
      d_ptr(new FileSelectionPagePrivate(this))
{
    this->registerField(INPUT_FILES_FIELD, d_ptr->m_ui->fileTreeWidget);
}

void FileSelectionPage::resizeEvent(QResizeEvent *event)
{
    Q_D(FileSelectionPage);
    d->resizeColumns();
    QWidget::resizeEvent(event);
}

bool FileSelectionPage::isComplete() const
{
    Q_D(const FileSelectionPage);
    return d->m_ui->fileTreeWidget->topLevelItemCount() != 0;
}

bool FileSelectionPage::validatePage()
{
    Q_D(FileSelectionPage);
    setField(INPUT_FILES_FIELD, QVariant::fromValue(d->m_fileNames));
    return d->m_ui->fileTreeWidget->topLevelItemCount() !=0;
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

const QString ECLIPSE_GRIDS = QLatin1String("eclipseGrids");
}

using syntheticSeismic::domain::EclipseGrid;

class EclipseGridImportPagePrivate
{
    explicit EclipseGridImportPagePrivate(EclipseGridImportPage* q);
    void updateWidget();
    void importEclipseFiles();
    void importEclipseFile(const QPair<QFileInfo, int>&);

    Q_DECLARE_PUBLIC(EclipseGridImportPage)
    EclipseGridImportPage* q_ptr;
    std::unique_ptr<Ui::EclipseGridImportPage> m_ui;
    QHash<QString, QVector<QString> > m_eclipseGridsErros;
    QHash<QString, QVector<QPair<QString, std::shared_ptr<EclipseGrid>>> > m_eclipseGrids;
    QMutex m_mutexEclipseGrids;
    bool m_eclipseFilesImported;
};

EclipseGridImportPagePrivate::EclipseGridImportPagePrivate(EclipseGridImportPage *q)
    : q_ptr(q)
    , m_ui(std::make_unique<Ui::EclipseGridImportPage>())
    , m_eclipseGrids()
    , m_eclipseFilesImported(false)
{
    m_ui->setupUi(q);

    const auto emitCompleteChangedFunction = [this](const double) -> void {
        Q_EMIT q_ptr->completeChanged();
    };

    QObject::connect(m_ui->regularGridXDimensionDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, emitCompleteChangedFunction);
    QObject::connect(m_ui->regularGridYDimensionDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, emitCompleteChangedFunction);
    QObject::connect(m_ui->regularGridZDimensionDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, emitCompleteChangedFunction);
}

void EclipseGridImportPagePrivate::updateWidget()
{
    importEclipseFiles();

    Q_Q(EclipseGridImportPage);

    m_ui->fileTreeWidget->clear();

    QList<QString> dirs = m_eclipseGrids.keys();
    std::sort(dirs.begin(), dirs.end());
    //m_eclipseFilesImported = true;

    for (int i = 0, countI = dirs.size(); i < countI; ++i) {

        const QString& dir = dirs.at(i);
        QTreeWidgetItem* topLevelItem = new QTreeWidgetItem({dir});

        m_ui->fileTreeWidget->addTopLevelItem(topLevelItem);

        const QVector<QPair<QString, std::shared_ptr<EclipseGrid>>> baseFileNames = m_eclipseGrids.value(dir);

        for (int j = 0, countJ = baseFileNames.size(); j < countJ; ++j) {
            const QString& fileName = baseFileNames.at(j).first;

            QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem({fileName});
            topLevelItem->addChild(treeWidgetItem);

//            QFileInfo fileInfo;
//            fileInfo.setFile(dir, fileName);

            //EclipseGridReader reader(fileInfo.filePath());
            const QString& error = m_eclipseGridsErros[dir][j];
            const std::shared_ptr<EclipseGrid>& eclipseGrid = m_eclipseGrids[dir][j].second;
            //const std::shared_ptr<EclipseGrid> eclipseGrid = std::make_shared<EclipseGrid>(reader.read(error));
            if (error.isEmpty()) {
                //m_eclipseGrids[dir][j].second = eclipseGrid;
                treeWidgetItem->setText(MESSAGE_COLUMN, ECLIPSE_FILE_IMPORTED);

                const QString text = QLatin1String("X: ") + QString::number(eclipseGrid->numberOfCellsInX()) +
                        QLatin1String(", Y: ") + QString::number(eclipseGrid->numberOfCellsInY()) +
                        QLatin1String(", Z: ") + QString::number(eclipseGrid->numberOfCellsInY());

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

            m_ui->fileTreeWidget->setItemWidget(treeWidgetItem, EXPORT_MAIN_SURFACE_COLUMN, exportSurfaceAction);
            // END EXPORT MAIN SURFACE

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

        m_eclipseGrids[dir][j].second = eclipseGrid;

        if (!error.isEmpty())
        {
            m_eclipseGridsErros[dir][j] = error;
            m_eclipseFilesImported = false;
        }

    m_mutexEclipseGrids.unlock();
}

void EclipseGridImportPagePrivate::importEclipseFiles()
{
    QList<QString> dirs = m_eclipseGrids.keys();
    std::sort(dirs.begin(), dirs.end());
    m_eclipseFilesImported = true;

    QVector<QPair<QFileInfo, int>> fileInfosForImport;

    for (int i = 0, countI = dirs.size(); i < countI; ++i) {

        const QString& dir = dirs.at(i);

        const QVector<QPair<QString, std::shared_ptr<EclipseGrid>>> baseFileNames = m_eclipseGrids.value(dir);

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

    registerField(ECLIPSE_GRIDS, d_ptr->m_ui->fileTreeWidget);
}

bool EclipseGridImportPage::validatePage()
{
    Q_D(EclipseGridImportPage);

    setField(REGULAR_GRID_X_DIMENSION, d_ptr->m_ui->regularGridXDimensionDoubleSpinBox->value());
    setField(REGULAR_GRID_Y_DIMENSION, d_ptr->m_ui->regularGridYDimensionDoubleSpinBox->value());
    setField(REGULAR_GRID_Z_DIMENSION, d_ptr->m_ui->regularGridZDimensionDoubleSpinBox->value());

    QVector<std::shared_ptr<EclipseGrid>> eclipseGrids;

    const QList<QString> dirs = d->m_eclipseGrids.keys();
    for (const QString& dir : dirs) {

        const QVector<QPair<QString, std::shared_ptr<EclipseGrid>>> pairs = d->m_eclipseGrids.value(dir);
        for (const QPair<QString, std::shared_ptr<EclipseGrid>>& pair : pairs) {
            const auto eclipsGrid = pair.second;
            Q_ASSERT(eclipsGrid);
            if (eclipsGrid) {
                eclipseGrids.push_back(eclipsGrid);
            }
        }
    }

    setField(ECLIPSE_GRIDS, QVariant::fromValue(eclipseGrids));

    return true;
}

void EclipseGridImportPage::initializePage()
{
    Q_D(EclipseGridImportPage);
    d->m_eclipseGrids.clear();
    d->m_eclipseGridsErros.clear();
    const QHash<QString, QSet<QString>> fileNames = field(INPUT_FILES_FIELD).value<QHash<QString, QSet<QString>>>();

    const QList<QString> dirs = fileNames.keys();
    for (const QString& dir : dirs) {
        const QSet<QString> files = fileNames.value(dir);
        for (const QString& file : files) {
            d->m_eclipseGridsErros[dir].push_back("");
            d->m_eclipseGrids[dir].push_back(QPair<QString, std::shared_ptr<syntheticSeismic::domain::EclipseGrid>>(file, std::shared_ptr<syntheticSeismic::domain::EclipseGrid>()));
        }
    }
    d->updateWidget();
}

bool EclipseGridImportPage::isComplete() const
{
    Q_D(const EclipseGridImportPage);
    return d->m_ui->fileTreeWidget->topLevelItemCount() != 0
            && d->m_eclipseFilesImported
            && !qFuzzyIsNull(d->m_ui->regularGridXDimensionDoubleSpinBox->value())
            && !qFuzzyIsNull(d->m_ui->regularGridYDimensionDoubleSpinBox->value())
            && !qFuzzyIsNull(d->m_ui->regularGridZDimensionDoubleSpinBox->value());
}

/************************************************************************************************************************************************************************/

using syntheticSeismic::domain::Lithology;
using syntheticSeismic::domain::LithologyDictionary;

class SegyCreationPagePrivate
{
    explicit SegyCreationPagePrivate(SegyCreationPage* q);
    void updateWidget();
    void showWidgetToAddLithology();
    void removeRow(const int row);
    int getRowVelocityTableWidget(QWidget* widget);

    Q_DECLARE_PUBLIC(SegyCreationPage)
    SegyCreationPage* q_ptr;
    std::unique_ptr<Ui::SegyCreationPage> m_ui;
    QVector<Lithology> m_lithologies;
    QVector<std::shared_ptr<EclipseGrid>> m_eclipseGrids;
    size_t m_numberOfCellsInX;
    size_t m_numberOfCellsInY;
    size_t m_numberOfCellsInZ;
};

SegyCreationPagePrivate::SegyCreationPagePrivate(SegyCreationPage *q)
    : q_ptr(q)
    , m_ui(std::make_unique<Ui::SegyCreationPage>())
    , m_lithologies()
    , m_eclipseGrids()
    , m_numberOfCellsInX(0)
    , m_numberOfCellsInY(0)
    , m_numberOfCellsInZ(0)
{
    m_ui->setupUi(q);
    m_lithologies = syntheticSeismic::domain::Facade::lithologyDictionary().lithologies();

    QObject::connect(m_ui->restoreDefaultVelocitiesPushButton, &QPushButton::clicked, q_ptr, [this](const bool){
        m_lithologies = syntheticSeismic::domain::Facade::lithologyDictionary().lithologies();
        updateWidget();
    });

    QObject::connect(m_ui->addVelocityPushButton, &QPushButton::clicked, q_ptr, [this](const bool){
        showWidgetToAddLithology();
    });

    QObject::connect(m_ui->rickerWaveletFrequencyDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, [this](const double){
        Q_EMIT q_ptr->completeChanged();
    });

    // BEGIN LITHOLOGY SEGY FILE
    QObject::connect(m_ui->lithologyFileNameLineEdit, &QLineEdit::textChanged, q_ptr, [this](const QString&) {
        Q_EMIT q_ptr->completeChanged();
    });
    QObject::connect(m_ui->outputLithologyFileNameToolButton, &QPushButton::clicked, q_ptr, [this](const bool) {
        const QString title = QObject::tr("Save Lithology SEG-Y file");
        const QString filter = QLatin1String("SEG-Y (*.segy)");
        const QString fileName = QFileDialog::getSaveFileName(q_ptr, title, QString(), filter);
        m_ui->lithologyFileNameLineEdit->setText(fileName);
        Q_EMIT q_ptr->completeChanged();
    });
    // END LITHOLOGY SEGY FILE
    // BEGIN IMPEDANCE SEGY FILE
    QObject::connect(m_ui->impedanceFileNameLineEdit, &QLineEdit::textChanged, q_ptr, [this](const QString&) {
        Q_EMIT q_ptr->completeChanged();
    });
    QObject::connect(m_ui->outputImpedanceFileNameToolButton, &QPushButton::clicked, q_ptr, [this](const bool) {
        const QString title = QObject::tr("Save Impedance SEG-Y file");
        const QString filter = QLatin1String("SEG-Y (*.segy)");
        const QString fileName = QFileDialog::getSaveFileName(q_ptr, title, QString(), filter);
        m_ui->impedanceFileNameLineEdit->setText(fileName);
        Q_EMIT q_ptr->completeChanged();
    });
    // END IMPEDANCE SEGY FILE
    // BEGIN REFLECTIVITY SEGY FILE
    QObject::connect(m_ui->reflectivityFileNameLineEdit, &QLineEdit::textChanged, q_ptr, [this](const QString&) {
        Q_EMIT q_ptr->completeChanged();
    });
    QObject::connect(m_ui->outputReflectivityFileNameToolButton, &QPushButton::clicked, q_ptr, [this](const bool) {
        const QString title = QObject::tr("Save Reflectivity SEG-Y file");
        const QString filter = QLatin1String("SEG-Y (*.segy)");
        const QString fileName = QFileDialog::getSaveFileName(q_ptr, title, QString(), filter);
        m_ui->reflectivityFileNameLineEdit->setText(fileName);
        Q_EMIT q_ptr->completeChanged();
    });
    // END REFLECTIVITY SEGY FILE
    // BEGIN AMPLITUDE SEGY FILE
    QObject::connect(m_ui->amplitudeFileNameLineEdit, &QLineEdit::textChanged, q_ptr, [this](const QString&){
        Q_EMIT q_ptr->completeChanged();
    });
    QObject::connect(m_ui->outputAmplitudeFileNameToolButton, &QPushButton::clicked, q_ptr, [this](const bool){
        const QString fileName = QFileDialog::getSaveFileName(q_ptr, QObject::tr("Save Time Seismic SEG-Y file"), QString(), QLatin1String("SEG-Y (*.segy)"));
        m_ui->amplitudeFileNameLineEdit->setText(fileName);
        Q_EMIT q_ptr->completeChanged();
    });
    // END AMPLITUDE SEGY FILE
    // BEGIN SEISMIC DEPTH AMPLITUDE SEGY FILE
    QObject::connect(m_ui->depthAmplitudeFileNameLineEdit, &QLineEdit::textChanged, q_ptr, [this](const QString&){
        Q_EMIT q_ptr->completeChanged();
    });
    QObject::connect(m_ui->outputDepthAmplitudeFileNameToolButton, &QPushButton::clicked, q_ptr, [this](const bool){
        const QString fileName = QFileDialog::getSaveFileName(q_ptr, QObject::tr("Save Depth Seismic SEG-Y file"), QString(), QLatin1String("SEG-Y (*.segy)"));
        m_ui->depthAmplitudeFileNameLineEdit->setText(fileName);
        Q_EMIT q_ptr->completeChanged();
    });
    // END AMPLITUDE SEGY FILE

    updateWidget();
}

void SegyCreationPagePrivate::showWidgetToAddLithology()
{
    Q_Q(SegyCreationPage);
    AddingVelocityWidget* dialog = new AddingVelocityWidget(m_lithologies, q);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->open();

    QObject::connect(dialog, &QDialog::accepted, q, [this, dialog]() {
        m_lithologies.push_back(dialog->lithology());
        dialog->close();
        updateWidget();
    });

    QObject::connect(dialog, &QDialog::rejected, q, [dialog]() {
        dialog->close();
    });
}

int SegyCreationPagePrivate::getRowVelocityTableWidget(QWidget* widget)
{
    for (int row = 0; row < m_ui->velocityTableWidget->rowCount(); ++row)
    {
        for (int col = 0; col < m_ui->velocityTableWidget->columnCount(); ++col)
        {
            if (m_ui->velocityTableWidget->cellWidget(row, col) == widget)
            {
                return row;
            }
        }
    }

    return -1;
}

void SegyCreationPagePrivate::updateWidget()
{
    Q_Q(SegyCreationPage);

    for (int row = 0, rowCount = m_ui->velocityTableWidget->rowCount(); row < rowCount; ++row) {
        for (int column = 0, columnCount = m_ui->velocityTableWidget->columnCount(); column < columnCount; ++column) {
            QWidget* widget = m_ui->velocityTableWidget->cellWidget(row, column);
            if (widget) {
                widget->deleteLater();
            }
        }
    }

    QSignalBlocker signalBlocker(m_ui->velocityTableWidget);

    while(m_ui->velocityTableWidget->rowCount() != 0) {
        m_ui->velocityTableWidget->removeRow(0);
    }

    m_ui->velocityTableWidget->setRowCount(m_lithologies.size());
    m_ui->velocityTableWidget->setColumnCount(5);

    for (int row = 0, rowCount = m_lithologies.size(); row < rowCount; ++row) {

        QTableWidgetItem* id = new QTableWidgetItem(QString::number(m_lithologies.at(row).getId()));
        id->setFlags(id->flags() & ~Qt::ItemIsEditable);
        QTableWidgetItem* name = new QTableWidgetItem();
        name->setFlags(name->flags() & ~Qt::ItemIsEditable);
        QTableWidgetItem* velocity = new QTableWidgetItem(QString::number(m_lithologies.at(row).getVelocity()));
        velocity->setFlags(velocity->flags() & ~Qt::ItemIsEditable);
        QTableWidgetItem* density = new QTableWidgetItem(QString::number(m_lithologies.at(row).getDensity()));
        density->setFlags(density->flags() & ~Qt::ItemIsEditable);
        QTableWidgetItem* removeAction = new QTableWidgetItem(REMOVE_ACTION_MESSAGE);
        removeAction->setFlags(density->flags() & ~Qt::ItemIsEditable);

        QSpinBox* idEditor = new QSpinBox(q_ptr);
        idEditor->setMaximum(INT_MAX);
        idEditor->setValue(m_lithologies[row].getId());
        QLineEdit* nameEditor = new QLineEdit(q_ptr);
        nameEditor->setText(m_lithologies[row].getName());
        QDoubleSpinBox* velocityEditor = new QDoubleSpinBox(q_ptr);
        velocityEditor->setMaximum(DBL_MAX);
        velocityEditor->setValue(m_lithologies[row].getVelocity());
        QDoubleSpinBox* densityEditor = new QDoubleSpinBox(q_ptr);
        densityEditor->setMaximum(DBL_MAX);
        densityEditor->setValue(m_lithologies[row].getDensity());
        QPushButton* removeButton = new QPushButton(q_ptr);
        removeButton->setIcon(QIcon(QLatin1String(":/remove")));

        m_ui->velocityTableWidget->setItem(row, 0, id);
        m_ui->velocityTableWidget->setItem(row, 1, name);
        m_ui->velocityTableWidget->setItem(row, 2, velocity);
        m_ui->velocityTableWidget->setItem(row, 3, density);
        m_ui->velocityTableWidget->setItem(row, 4, removeAction);

        m_ui->velocityTableWidget->setCellWidget(row, 0, idEditor);
        m_ui->velocityTableWidget->setCellWidget(row, 1, nameEditor);
        m_ui->velocityTableWidget->setCellWidget(row, 2, velocityEditor);
        m_ui->velocityTableWidget->setCellWidget(row, 3, densityEditor);
        m_ui->velocityTableWidget->setCellWidget(row, 4, removeButton);

        QObject::connect(idEditor, QOverload<int>::of(&QSpinBox::valueChanged), [this, idEditor](const int& value) {
            const int& row = getRowVelocityTableWidget(idEditor);

            if (m_lithologies.size() > row && row != -1)
            {
                m_lithologies[row].setId(value);
            }
        });

        QObject::connect(nameEditor, &QLineEdit::textChanged, [this, nameEditor](const QString& value) {
            const int& row = getRowVelocityTableWidget(nameEditor);

            if (m_lithologies.size() > row && row != -1)
            {
                m_lithologies[row].setName(value);
            }
        });

        QObject::connect(velocityEditor, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, velocityEditor](const double& value) {
            const int& row = getRowVelocityTableWidget(velocityEditor);

            if (m_lithologies.size() > row && row != -1)
            {
                m_lithologies[row].setVelocity(value);
            }
        });

        QObject::connect(densityEditor, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, densityEditor](const double& value) {
            const int& row = getRowVelocityTableWidget(densityEditor);

            if (m_lithologies.size() > row && row != -1)
            {
                m_lithologies[row].setDensity(value);
            }
        });

        QObject::connect(removeButton, &QPushButton::clicked, [this, removeButton](const bool) {
            const int& row = getRowVelocityTableWidget(removeButton);

            if (m_lithologies.size() > row && row != -1)
            {
                // removing lithology from lithologies
                removeRow(row);
                m_lithologies.remove(row);
            }
        });
    }

    m_ui->velocityTableWidget->resizeColumnsToContents();
    if (q->isVisible()) {
        Q_EMIT q->completeChanged();
    }
}

void SegyCreationPagePrivate::removeRow(const int row)
{
    if (m_ui->velocityTableWidget->rowCount() == 0) {
        return;
    }

    QWidget* widget = m_ui->velocityTableWidget->cellWidget(row, 4);
    if (widget) {
        widget->deleteLater();
    }

    if (row < m_ui->velocityTableWidget->rowCount()) {
        m_ui->velocityTableWidget->removeRow(row);
    }
}

Worker::Worker(SegyCreationPage* segyCreationPage) : m_segyCreationPage(segyCreationPage)
{
    connect(m_segyCreationPage, &SegyCreationPage::progressUpdated, this, &Worker::progressUpdated);
    connect(m_segyCreationPage, &SegyCreationPage::processFinished, this, &Worker::finished);
}

void Worker::run()
{
    m_segyCreationPage->process();
}

SegyCreationPage::SegyCreationPage(QWidget* parent)
    : QWizardPage(parent),
        m_processThread(nullptr),
        m_progressDialog(nullptr),
        m_processWorker(nullptr),
        d_ptr(new SegyCreationPagePrivate(this))
{
}

SegyCreationPage::~SegyCreationPage()
{
}

void SegyCreationPage::initProcessThread()
{
    m_processThread = new QThread();
    m_processWorker = new Worker(this);
    m_progressDialog = new QProgressDialog("Processing...", nullptr, 0, 100, this);
    m_progressDialog->setWindowTitle("Processing");
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setMinimumDuration(0);
    m_progressDialog->setWindowFlags(m_progressDialog->windowFlags() & ~Qt::WindowCloseButtonHint);

    connect(m_processWorker, &Worker::progressUpdated, m_progressDialog, &QProgressDialog::setValue);
    connect(m_processWorker, &Worker::finished, m_progressDialog, &QProgressDialog::reset);
    connect(m_processWorker, &Worker::finished, m_processThread, &QThread::quit);
    connect(m_progressDialog, &QProgressDialog::canceled, m_processThread, &QThread::quit);
    connect(m_processThread, &QThread::finished, m_processWorker, &QObject::deleteLater);
    connect(m_processThread, &QThread::finished, m_processThread, &QThread::deleteLater);
    connect(m_processThread, &QThread::finished, m_progressDialog, &QProgressDialog::deleteLater);
    connect(m_processThread, &QThread::finished, this, &SegyCreationPage::showProcessMessage);

    m_processWorker->moveToThread(m_processThread);
}

void SegyCreationPage::showProcessMessage()
{
    if (m_processMessage.first == QMessageBox::Warning)
    {
        QMessageBox::warning(QApplication::activeWindow(), tr("SyntheticSeis - Error"), m_processMessage.second, QMessageBox::NoButton);
    }
    else if (m_processMessage.first == QMessageBox::Information)
    {
        QMessageBox::information(QApplication::activeWindow(), tr("SyntheticSeis - Success"), m_processMessage.second, QMessageBox::NoButton);
    }
}

bool SegyCreationPage::isComplete() const
{
    Q_D(const SegyCreationPage);

    return !d->m_lithologies.isEmpty()
            && !qFuzzyIsNull(d->m_ui->rickerWaveletFrequencyDoubleSpinBox->value())
            && (!d->m_ui->amplitudeFileNameLineEdit->text().isEmpty() ||
               !d->m_ui->depthAmplitudeFileNameLineEdit->text().isEmpty());
}

void SegyCreationPage::process()
{
    Q_D(SegyCreationPage);

    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::storage;
    using syntheticSeismic::geometry::Volume;

    try
    {
        const double undefinedImpedance = 2.500;
        const auto undefinedLithology = std::make_shared<Lithology>(0, "undefined", 2500, 1);

        std::vector<std::shared_ptr<Volume>> allVolumes;
        for (const auto& item : d->m_eclipseGrids)
        {
            const std::vector<std::shared_ptr<Volume>> volumesOfFirstLayer = ExtractVolumes::extractFirstLayerFrom(*item);
            const std::vector<std::shared_ptr<Volume>> volumes = ExtractVolumes::extractFromVolumesOfFirstLayer(volumesOfFirstLayer, *item, true);
            allVolumes.insert(allVolumes.end(), volumes.begin(), volumes.end());
        }

        emit progressUpdated(5);

        const auto minimumRectangle = ExtractMinimumRectangle2D::extractFrom(allVolumes);
        const auto rotateResult = RotateVolumeCoordinate::rotateByMinimumRectangle(allVolumes, minimumRectangle);

        VolumeToRegularGrid volumeToRegularGrid(d->m_numberOfCellsInX, d->m_numberOfCellsInY, d->m_numberOfCellsInZ);
        auto regularGridInMeters = volumeToRegularGrid.convertVolumesToRegularGrid(
            allVolumes, minimumRectangle, rotateResult->minimumZ, rotateResult->maximumZ
            );

        emit progressUpdated(25);

        ConvertRegularGridCalculator convertGrid(undefinedLithology);
        for (const auto &item : d->m_lithologies)
        {
            convertGrid.addLithology(std::make_shared<Lithology>(item));
        }
        auto regularGridInSeconds = convertGrid.fromZInMetersToZInSeconds(regularGridInMeters);

        auto waveletStep = regularGridInSeconds.getCellSizeInZ() * 1000;
        std::cout << "Wavelet Step: " << waveletStep << std::endl;

        RickerWaveletCalculator rickerWaveletCalculator;
        rickerWaveletCalculator.setFrequency(25);
        rickerWaveletCalculator.setStep(waveletStep);
        const auto wavelet = rickerWaveletCalculator.extract();

        emit progressUpdated(40);

        const QString lithologyPath = d->m_ui->lithologyFileNameLineEdit->text();
        if (!lithologyPath.isEmpty())
        {
            RegularGrid<int> lithologyRegularGrid(
                regularGridInSeconds.getNumberOfCellsInX(), regularGridInSeconds.getNumberOfCellsInY(), regularGridInSeconds.getNumberOfCellsInZ(),
                regularGridInSeconds.getCellSizeInX(), regularGridInSeconds.getCellSizeInY(), regularGridInSeconds.getCellSizeInZ(),
                EnumUnit::Meters, EnumUnit::Meters, EnumUnit::Seconds,
                regularGridInSeconds.getRectanglePoints(), regularGridInSeconds.getZBottom(), regularGridInSeconds.getZTop(),
                0, 0
                );
            auto &data = lithologyRegularGrid.getData();
            for (size_t i = 0; i < regularGridInSeconds.getNumberOfCellsInX(); ++i)
            {
                for (size_t j = 0; j < regularGridInSeconds.getNumberOfCellsInY(); ++j)
                {
                    for (size_t k = 0; k < regularGridInSeconds.getNumberOfCellsInZ(); ++k)
                    {
                        data[i][j][k] = regularGridInSeconds.getData(i, j, k) == nullptr ? EclipseGrid::NoDataValue : regularGridInSeconds.getData(i, j, k)->idLithology;
                    }
                }
            }
            const QString hdf5Path = lithologyPath + ".h5";
            RegularGridHdf5Storage<int> hdf5Storage(hdf5Path, "data");
            hdf5Storage.write(lithologyRegularGrid);

            SegyWriter segyWriter(lithologyPath);
            segyWriter.writeByHdf5File(hdf5Path);
        }

        emit progressUpdated(50);

        ImpedanceRegularGridCalculator impedanceCalculator(undefinedLithology);
        for (const auto &item : d->m_lithologies)
        {
            impedanceCalculator.addLithology(std::make_shared<Lithology>(item));
        }
        auto impedanceRegularGrid = impedanceCalculator.execute(regularGridInSeconds);
        const QString impedancePath = d->m_ui->impedanceFileNameLineEdit->text();
        if (!impedancePath.isEmpty())
        {
            const QString hdf5Path = impedancePath + ".h5";
            RegularGridHdf5Storage<double> hdf5Storage(impedancePath + ".h5", "data");
            hdf5Storage.write(*impedanceRegularGrid);

            SegyWriter segyWriter(impedancePath);
            segyWriter.writeByHdf5File(hdf5Path);
        }

        emit progressUpdated(60);

        ReflectivityRegularGridCalculator reflectivityCalculator(undefinedImpedance);
        auto reflectivityRegularGrid = reflectivityCalculator.execute(*impedanceRegularGrid);
        impedanceRegularGrid.reset();

        const QString reflectivityPath = d->m_ui->reflectivityFileNameLineEdit->text();
        if (!reflectivityPath.isEmpty())
        {
            const QString hdf5Path = reflectivityPath + ".h5";
            RegularGridHdf5Storage<double> storage(hdf5Path, "data");
            storage.write(*reflectivityRegularGrid);

            SegyWriter segyWriter(reflectivityPath);
            segyWriter.writeByHdf5File(hdf5Path);
        }

        emit progressUpdated(75);

        ConvolutionRegularGridCalculator convolutionCalculator;
        auto amplitudeRegularGrid = convolutionCalculator.execute(*reflectivityRegularGrid, *wavelet);
        reflectivityRegularGrid.reset();

        const QString amplitudePath = d->m_ui->amplitudeFileNameLineEdit->text();
        if (!amplitudePath.isEmpty())
        {
            const QString hdf5Path = amplitudePath + ".h5";
            RegularGridHdf5Storage<double> storage(hdf5Path, "data");
            storage.write(*amplitudeRegularGrid);

            SegyWriter segyWriter(amplitudePath);
            segyWriter.writeByHdf5File(hdf5Path);
        }

        emit progressUpdated(90);

        const QString depthAmplitudePath = d->m_ui->depthAmplitudeFileNameLineEdit->text();
        if (!depthAmplitudePath.isEmpty())
        {
            auto depthAmplitudeRegularGrid = convertGrid.fromZInSecondsToZInMeters(regularGridInSeconds, *amplitudeRegularGrid);

            const QString hdf5Path = depthAmplitudePath + ".h5";
            RegularGridHdf5Storage<double> storage(hdf5Path, "data");
            storage.write(depthAmplitudeRegularGrid);

            SegyWriter segyWriter(depthAmplitudePath);
            segyWriter.writeByHdf5File(hdf5Path);
        }

        m_processMessage.first = QMessageBox::Information;
        m_processMessage.second = "Process completed successfully.";

        emit progressUpdated(100);
    }
    catch (std::exception &e)
    {
        m_processMessage.first = QMessageBox::Warning;
        m_processMessage.second = e.what();

        emit progressUpdated(100);
    }

    emit processFinished();
}

bool SegyCreationPage::validatePage()
{
    initProcessThread();
    m_progressDialog->show();

    connect(m_processThread, &QThread::started, m_processWorker, &Worker::run);
    m_processThread->start();

    // Returns false to not close the dialog
    return false;
}

void SegyCreationPage::initializePage()
{
    Q_D(SegyCreationPage);
    d->m_eclipseGrids = field(ECLIPSE_GRIDS).value<QVector<std::shared_ptr<EclipseGrid>>>();

    d->m_numberOfCellsInX = field(REGULAR_GRID_X_DIMENSION).value<size_t>();
    d->m_numberOfCellsInY = field(REGULAR_GRID_Y_DIMENSION).value<size_t>();
    d->m_numberOfCellsInZ = field(REGULAR_GRID_Z_DIMENSION).value<size_t>();
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/

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


