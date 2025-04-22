#include "Wizard.h"

#include "ui_AddingVelocityWidget.h"
#include "ui_EclipseGridImportPage.h"

#include "FileSelectionPage.h"

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
#include "SegyCreationPage.h"

using syntheticSeismic::domain::Lithology;
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

    for (int i = 0, countI = dirs.size(); i < countI; ++i) {

        const QString& dir = dirs.at(i);
        QTreeWidgetItem* topLevelItem = new QTreeWidgetItem({dir});

        m_ui->fileTreeWidget->addTopLevelItem(topLevelItem);

        const QVector<QPair<QString, std::shared_ptr<EclipseGrid>>> baseFileNames = m_eclipseGrids.value(dir);

        for (int j = 0, countJ = baseFileNames.size(); j < countJ; ++j) {
            const QString& fileName = baseFileNames.at(j).first;

            QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem({fileName});
            topLevelItem->addChild(treeWidgetItem);

            const QString& error = m_eclipseGridsErros[dir][j];
            const std::shared_ptr<EclipseGrid>& eclipseGrid = m_eclipseGrids[dir][j].second;
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
            m_ui->regularGridXDimensionDoubleSpinBox->setValue(eclipseGrid->numberOfCellsInX());
            m_ui->regularGridYDimensionDoubleSpinBox->setValue(eclipseGrid->numberOfCellsInY());
            m_ui->regularGridZDimensionDoubleSpinBox->setValue(eclipseGrid->numberOfCellsInZ());
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


