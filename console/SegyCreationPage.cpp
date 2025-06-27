#include <QDebug>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "ui_SegyCreationPage.h"
#include "SegyCreationPage.h"
#include "domain/src/ExtractVolumes.h"
#include "domain/src/LithologyDictionary.h"
#include "domain/src/EclipseGrid.h"
#include "domain/src/ExtractMinimumRectangle2D.h"
#include "domain/src/RotateVolumeCoordinate.h"
#include "domain/src/VolumeToRegularGrid.h"
#include "domain/src/ConvertRegularGridCalculator.h"
#include "domain/src/Facade.h"
#include "Wizard.h"
#include "domain/src/RickerWaveletCalculator.h"
#include "storage/src/RegularGridHdf5Storage.h"
#include "storage/src/writer/SEGYWriter.h"
#include "domain/src/ImpedanceRegularGridCalculator.h"
#include "domain/src/ReflectivityRegularGridCalculator.h"
#include "domain/src/ConvolutionRegularGridCalculator.h"

using syntheticSeismic::domain::EclipseGrid;
using syntheticSeismic::domain::Lithology;
using syntheticSeismic::domain::LithologyDictionary;

namespace {
const QString REGULAR_GRID_X_DIMENSION = QLatin1String("regularGridXDimension");
const QString REGULAR_GRID_Y_DIMENSION = QLatin1String("regularGridYDimension");
const QString REGULAR_GRID_Z_DIMENSION = QLatin1String("regularGridZDimension");


const QString CELL_SIZE_IN_X = QLatin1String("cellSizeInX");
const QString CELL_SIZE_IN_Y = QLatin1String("cellSizeInY");
const QString CELL_SIZE_IN_Z = QLatin1String("cellSizeInZ");
const QString RICKER_WAVELET_FREQUENCY = QLatin1String("rickerWaveletFrequency");

const QString ECLIPSE_GRIDS = QLatin1String("eclipseGrids");
const QString ALL_VOLUMES = QLatin1String("allVolumes");
}

namespace syntheticSeismic {
namespace widgets {
QString fillingTypeToString(const domain::FillingType& type)
{
    switch (type)
    {
        case domain::FillingType::None: return "None";
        case domain::FillingType::Top: return "Top";
        case domain::FillingType::Bottom: return "Bottom";
        case domain::FillingType::Both: return "Both";
        default: return "Unknown";
    }
}

class SegyCreationPagePrivate
{
    explicit SegyCreationPagePrivate(SegyCreationPage* q);
    void updateWidget();
    void showWidgetToAddLithology();
    void removeRow(const int row);
    int getRowVelocityTableWidget(QWidget* widget);
    void loadDictionary(const QString &filePath);
    void saveDictionary(const QString &filePath);

    Q_DECLARE_PUBLIC(SegyCreationPage)
    SegyCreationPage* q_ptr;
    std::unique_ptr<Ui::SegyCreationPage> m_ui;
    QVector<Lithology> m_lithologies;
    QVector<std::shared_ptr<EclipseGrid>> m_eclipseGrids;
    size_t m_numberOfCellsInX;
    size_t m_numberOfCellsInY;
    size_t m_numberOfCellsInZ;
    double m_cellSizeInX;
    double m_cellSizeInY;
    double m_cellSizeInZ;
    double m_rickerWaveletFrequency;
};

SegyCreationPagePrivate::SegyCreationPagePrivate(SegyCreationPage *q)
        : q_ptr(q)
        , m_ui(std::make_unique<Ui::SegyCreationPage>())
        , m_lithologies()
        , m_eclipseGrids()
        , m_numberOfCellsInX(0)
        , m_numberOfCellsInY(0)
        , m_numberOfCellsInZ(0)
        , m_cellSizeInX(0.0)
        , m_cellSizeInY(0.0)
        , m_cellSizeInZ(0.0)
        , m_rickerWaveletFrequency(0.0)
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


    QObject::connect(m_ui->loadDictionaryPushButton, &QPushButton::clicked, q_ptr, [this](const bool) {
        const auto title = QObject::tr("Open Dictionary");
        const auto filter = QLatin1String("SyntheticSeis Dictionary (*.ssjson)");

        const auto filePath = QFileDialog::getOpenFileName(q_ptr, title, "", filter);

        loadDictionary(filePath);

        Q_EMIT q_ptr->completeChanged();
    });

    QObject::connect(m_ui->saveDictionaryPushButton, &QPushButton::clicked, q_ptr, [this](const bool) {
        const auto title = QObject::tr("Save Dictionary");
        const auto filter = QLatin1String("SyntheticSeis Dictionary (*.ssjson)");

        const auto filePath = QFileDialog::getSaveFileName(q_ptr, title, "", filter);

        saveDictionary(filePath);

        Q_EMIT q_ptr->completeChanged();
    });

    //QObject::connect(m_ui->rickerWaveletFrequencyDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), q_ptr, [this](const double){
    //    Q_EMIT q_ptr->completeChanged();
    //});

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

    for (int row = 0, rowCount = m_lithologies.size(); row < rowCount; ++row)
    {
        auto idEditor = new QSpinBox(q_ptr);
        idEditor->setMaximum(INT_MAX);
        idEditor->setValue(m_lithologies[row].getId());
        auto nameEditor = new QLineEdit(q_ptr);
        nameEditor->setText(m_lithologies[row].getName());
        auto velocityEditor = new QDoubleSpinBox(q_ptr);
        velocityEditor->setMaximum(DBL_MAX);
        velocityEditor->setValue(m_lithologies[row].getVelocity());
        velocityEditor->setDecimals(0);
        auto densityEditor = new QDoubleSpinBox(q_ptr);
        densityEditor->setMaximum(DBL_MAX);
        densityEditor->setValue(m_lithologies[row].getDensity());
        densityEditor->setDecimals(0);
        auto fillingTypeEditor = new QComboBox(q_ptr);
        fillingTypeEditor->addItem(fillingTypeToString(domain::FillingType::None));
        fillingTypeEditor->addItem(fillingTypeToString(domain::FillingType::Top));
        fillingTypeEditor->addItem(fillingTypeToString(domain::FillingType::Bottom));
        fillingTypeEditor->addItem(fillingTypeToString(domain::FillingType::Both));
        fillingTypeEditor->setCurrentIndex(static_cast<int>(m_lithologies[row].getFillingType()));

        auto removeButton = new QPushButton(q_ptr);
        removeButton->setIcon(QIcon(QLatin1String(":/remove")));

        m_ui->velocityTableWidget->setCellWidget(row, 0, idEditor);
        m_ui->velocityTableWidget->setCellWidget(row, 1, nameEditor);
        m_ui->velocityTableWidget->setCellWidget(row, 2, velocityEditor);
        m_ui->velocityTableWidget->setCellWidget(row, 3, densityEditor);
        m_ui->velocityTableWidget->setCellWidget(row, 4, fillingTypeEditor);
        m_ui->velocityTableWidget->setCellWidget(row, 5, removeButton);

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

        QObject::connect(fillingTypeEditor, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, fillingTypeEditor](const int& value) {
            const int& row = getRowVelocityTableWidget(fillingTypeEditor);

            if (m_lithologies.size() > row && row != -1)
            {
                m_lithologies[row].setFillingType((domain::FillingType)value);
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

void SegyCreationPagePrivate::loadDictionary(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isArray()) {
        qWarning() << "JSON is not an array!";
        return;
    }

    m_lithologies.clear();

    QJsonArray jsonArray = doc.array();
    for (const QJsonValue &value : jsonArray) {
        if (!value.isObject()) continue;

        QJsonObject obj = value.toObject();

        const auto fillingType = (domain::FillingType) obj.value("fillingType").toInt();

        Lithology lithology(
                obj.value("id").toInt(),
                obj.value("name").toString(),
                obj.value("velocity").toDouble(),
                obj.value("density").toDouble(),
                fillingType
        );
        m_lithologies.push_back(lithology);
    }

    updateWidget();
}

void SegyCreationPagePrivate::saveDictionary(const QString &filePath)
{
    QJsonArray jsonArray;

    for (const auto &lithology : m_lithologies) {
        QJsonObject jsonObj;
        jsonObj["id"] = lithology.getId();
        jsonObj["name"] = lithology.getName();
        jsonObj["velocity"] = lithology.getVelocity();
        jsonObj["density"] = lithology.getDensity();
        jsonObj["fillingType"] = static_cast<int>(lithology.getFillingType());

        jsonArray.append(jsonObj);
    }

    QJsonDocument doc(jsonArray);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << file.errorString();
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "JSON saved successfully to" << filePath;
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
           //&& !qFuzzyIsNull(d->m_ui->rickerWaveletFrequencyDoubleSpinBox->value())
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

        std::vector<std::shared_ptr<Volume>> allVolumes = field(ALL_VOLUMES).value<std::vector<std::shared_ptr<Volume>>>();
        /*for (const auto& item : d->m_eclipseGrids)
        {
            const std::vector<std::shared_ptr<Volume>> volumesOfFirstLayer = ExtractVolumes::extractFirstLayerFrom(*item);
            const std::vector<std::shared_ptr<Volume>> volumes = ExtractVolumes::extractFromVolumesOfFirstLayer(volumesOfFirstLayer, *item, true);
            allVolumes.insert(allVolumes.end(), volumes.begin(), volumes.end());
        }*/

        emit progressUpdated(5);

        const auto minimumRectangle = ExtractMinimumRectangle2D::extractFrom(allVolumes);
        const auto rotateResult = RotateVolumeCoordinate::rotateByMinimumRectangle(allVolumes, minimumRectangle);

        VolumeToRegularGrid volumeToRegularGrid(d->m_numberOfCellsInX, d->m_numberOfCellsInY, d->m_numberOfCellsInZ, d->m_cellSizeInX, d->m_cellSizeInY, d->m_cellSizeInZ);

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

        std::cout << "cell size z orig: " << regularGridInMeters.getCellSizeInZ() << std::endl;
        // filling with two lithologies
        const auto& topLithology = createFillingLithology(FillingType::Top);
        const auto& bottomLithology = createFillingLithology(FillingType::Bottom);
        auto filledRegularGridInSeconds = convertGrid.fillTopBottomLithologyTimeGrid(regularGridInSeconds, topLithology, bottomLithology);

        auto waveletStep = filledRegularGridInSeconds.getCellSizeInZ() * 1000.0;
        std::cout << "Wavelet Step: " << waveletStep << std::endl;

        RickerWaveletCalculator rickerWaveletCalculator;
        rickerWaveletCalculator.setFrequency(d->m_rickerWaveletFrequency);
        rickerWaveletCalculator.setStep(waveletStep);
        const auto wavelet = rickerWaveletCalculator.extract();

        emit progressUpdated(40);

        const QString lithologyPath = d->m_ui->lithologyFileNameLineEdit->text();
        if (!lithologyPath.isEmpty())
        {
            RegularGrid<int> lithologyRegularGrid(
                    regularGridInMeters.getNumberOfCellsInX(),
                    regularGridInMeters.getNumberOfCellsInY(),
                    regularGridInMeters.getNumberOfCellsInZ(),
                    regularGridInMeters.getCellSizeInX(),
                    regularGridInMeters.getCellSizeInY(),
                    regularGridInMeters.getCellSizeInZ(),
                    regularGridInMeters.getUnitInX(),
                    regularGridInMeters.getUnitInY(),
                    regularGridInMeters.getUnitInZ(),
                    regularGridInMeters.getRectanglePoints(),
                    regularGridInMeters.getZBottom(),
                    regularGridInMeters.getZTop(),
                    0, 0
            );
            auto &data = lithologyRegularGrid.getData();
            for (size_t i = 0; i < regularGridInMeters.getNumberOfCellsInX(); ++i)
            {
                for (size_t j = 0; j < regularGridInMeters.getNumberOfCellsInY(); ++j)
                {
                    for (size_t k = 0; k < regularGridInMeters.getNumberOfCellsInZ(); ++k)
                    {
                        data[i][j][k] = regularGridInMeters.getData(i, j, k) == nullptr ? EclipseGrid::NoDataValue : regularGridInMeters.getData(i, j, k)->idLithology;
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
        auto impedanceRegularGrid = impedanceCalculator.execute(filledRegularGridInSeconds);
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
            auto depthAmplitudeRegularGrid = convertGrid.fromZInSecondsToZInMeters(filledRegularGridInSeconds, *amplitudeRegularGrid);

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

std::shared_ptr<domain::Lithology> SegyCreationPage::createFillingLithology(const domain::FillingType& fillingType)
{
    Q_D(const SegyCreationPage);

    for (const Lithology& lith : d->m_lithologies)
    {
        const domain::FillingType& fillingTypeCurr = lith.getFillingType();

        if (fillingTypeCurr == domain::FillingType::None)
        {
            continue;
        }

        if (fillingTypeCurr == fillingType || fillingTypeCurr == domain::FillingType::Both)
        {
            return std::make_shared<Lithology>(lith);
        }
    }

    return std::make_shared<Lithology>();
}

bool SegyCreationPage::validateLithologies()
{
    Q_D(const SegyCreationPage);

    int countFillingTop = 0;
    int countFillingBottom = 0;

    for (const Lithology& lith : d->m_lithologies)
    {
        const domain::FillingType& fillingType = lith.getFillingType();

        if (fillingType == domain::FillingType::Top)
        {
            countFillingTop++;
        }
        else if (fillingType == domain::FillingType::Bottom)
        {
            countFillingBottom++;
        }
        else if (fillingType == domain::FillingType::Both)
        {
            countFillingTop++;
            countFillingBottom++;
        }
    }

    m_processMessage.second.clear();

    if (countFillingTop != 1)
    {
        m_processMessage.first = QMessageBox::Information;
        m_processMessage.second = "There must be exactly 1 'Filling Top'";
    }

    if (countFillingBottom != 1)
    {
        if (!m_processMessage.second.isEmpty())
        {
            m_processMessage.second += "\n";
        }

        m_processMessage.first = QMessageBox::Information;
        m_processMessage.second += "There must be exactly 1 'Filling Bottom'";
    }

    return m_processMessage.second.isEmpty();
}

bool SegyCreationPage::validatePage()
{
    if (!validateLithologies())
    {
        showProcessMessage();

        return false;
    }

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
    d->m_cellSizeInX = field(CELL_SIZE_IN_X).value<double>();
    d->m_cellSizeInY = field(CELL_SIZE_IN_Y).value<double>();
    d->m_cellSizeInZ = field(CELL_SIZE_IN_Z).value<double>();
    d->m_rickerWaveletFrequency = field(RICKER_WAVELET_FREQUENCY).value<double>();
}

} // widgets
} // syntheticSeismic

Q_DECLARE_METATYPE(syntheticSeismic::geometry::Volume)
Q_DECLARE_METATYPE(std::shared_ptr<syntheticSeismic::geometry::Volume>)
Q_DECLARE_METATYPE(std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>>)
