#include "Wizard.h"

#include "ui_FileSelectionPage.h"
#include "ui_ProcessingPage.h"
#include "ui_RegularGridImportPage.h"

#include <QFileDialog>

#include "storage/src/reader/EclipseGridReader.h"

#include <memory>

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
    RegularGridImportPage* importPage = new RegularGridImportPage(q);
    ProcessingPage* processingPage = new ProcessingPage(q);

    q->addPage(fileSelectionPage);
    q->addPage(importPage);
    q->addPage(processingPage);

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


    QObject::connect(m_ui->cleanSelectedFilesPushButton, &QPushButton::clicked, q_ptr, [this](const bool){
        m_fileNames.clear();
        updateWidget();
    });
}

int FileSelectionPagePrivate::calculateActionMessageWidth () const
{
    Q_Q(const FileSelectionPage);
    return QFontMetrics(q->font()).width(REMOVE_ACTION_MESSAGE) * 2;
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

    const int removeActionMessageWidth = this->calculateActionMessageWidth();

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

            QPushButton* removeAction = new QPushButton(REMOVE_ACTION_MESSAGE, q);
            removeAction->setIcon(QIcon(QLatin1String(":/images/remove")));
            removeAction->setMaximumWidth(removeActionMessageWidth);

            QObject::connect(removeAction, &QPushButton::clicked, q, [this, item](const bool){
                this->removeItem(item);
            });

            m_ui->fileTreeWidget->setItemWidget(item, ACTION_COLUMN, removeAction);
        }
    }

    m_ui->fileTreeWidget->expandAll();
    resizeColumns();
    Q_EMIT q->completeChanged();
}

void FileSelectionPagePrivate::resizeColumns()
{
    Q_Q(FileSelectionPage);
    const int width = q->width();
    const int actionMessageWidth = this->calculateActionMessageWidth();
    m_ui->fileTreeWidget->setColumnWidth(0, width - actionMessageWidth * 3/2);
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
const QString ECLIPSE_FILE_IMPORTED = QObject::tr("Imported");


const QString REGULAR_GRID_X_DIMENSION = QLatin1String("regularGridXDimension");
const QString REGULAR_GRID_Y_DIMENSION = QLatin1String("regularGridYDimension");
const QString REGULAR_GRID_Z_DIMENSION = QLatin1String("regularGridZDimension");

const QString ECLIPSE_GRIDS = QLatin1String("eclipseGrids");
}

using syntheticSeismic::domain::EclipseGrid;

class RegularGridImportPagePrivate
{
    explicit RegularGridImportPagePrivate(RegularGridImportPage* q);
    void updateWidget();

    Q_DECLARE_PUBLIC(RegularGridImportPage)
    RegularGridImportPage* q_ptr;
    std::unique_ptr<Ui::RegularGridImportPage> m_ui;
    QHash<QString, QVector<QPair<QString, std::shared_ptr<EclipseGrid>>> > m_eclipseGrids;
    bool m_eclipseFilesImported;
};

RegularGridImportPagePrivate::RegularGridImportPagePrivate(RegularGridImportPage *q)
    : q_ptr(q)
    , m_ui(std::make_unique<Ui::RegularGridImportPage>())
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

void RegularGridImportPagePrivate::updateWidget()
{
    Q_Q(RegularGridImportPage);

    m_ui->fileTreeWidget->clear();

    QList<QString> dirs = m_eclipseGrids.keys();
    std::sort(dirs.begin(), dirs.end());
    m_eclipseFilesImported = true;

    for (int i = 0, countI = dirs.size(); i < countI; ++i) {

        const QString& dir = dirs.at(i);
        QTreeWidgetItem* topLevelItem = new QTreeWidgetItem({dir});

        m_ui->fileTreeWidget->addTopLevelItem(topLevelItem);

        const QVector<QPair<QString, std::shared_ptr<EclipseGrid>>> baseFileNames = m_eclipseGrids.value(dir);

        for (int j = 0, countJ = baseFileNames.size(); j < countJ; ++j) {
            const QString& fileName = baseFileNames.at(j).first;

            QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem({fileName});
            topLevelItem->addChild(treeWidgetItem);

            QFileInfo fileInfo;
            fileInfo.setFile(dir, fileName);

            syntheticSeismic::storage::EclipseGridReader reader(fileInfo.filePath());
            QString error;
            const std::shared_ptr<EclipseGrid> eclipseGrid = std::make_shared<EclipseGrid>(reader.read(error));
            if (error.isEmpty()) {
                m_eclipseGrids[dir][j].second = eclipseGrid;
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
        }
    }

    m_ui->fileTreeWidget->expandAll();
    m_ui->fileTreeWidget->resizeColumnToContents(0);
    Q_EMIT q->completeChanged();
}

/*------------------------------------------------------------------------------------------------------------------------*/

RegularGridImportPage::RegularGridImportPage(QWidget* parent)
    : QWizardPage(parent),
      d_ptr(new RegularGridImportPagePrivate(this))
{
    registerField(REGULAR_GRID_X_DIMENSION, d_ptr->m_ui->regularGridXDimensionDoubleSpinBox);
    registerField(REGULAR_GRID_Y_DIMENSION, d_ptr->m_ui->regularGridYDimensionDoubleSpinBox);
    registerField(REGULAR_GRID_Z_DIMENSION, d_ptr->m_ui->regularGridZDimensionDoubleSpinBox);

    registerField(ECLIPSE_GRIDS, d_ptr->m_ui->fileTreeWidget);
}

bool RegularGridImportPage::validatePage()
{
    Q_D(RegularGridImportPage);

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

void RegularGridImportPage::initializePage()
{
    Q_D(RegularGridImportPage);
    d->m_eclipseGrids.clear();
    const QHash<QString, QSet<QString>> fileNames = field(INPUT_FILES_FIELD).value<QHash<QString, QSet<QString>>>();

    const QList<QString> dirs = fileNames.keys();
    for (const QString& dir : dirs) {
        const QSet<QString> files = fileNames.value(dir);
        for (const QString& file : files) {
            d->m_eclipseGrids[dir].push_back(QPair<QString, std::shared_ptr<syntheticSeismic::domain::EclipseGrid>>(file, std::shared_ptr<syntheticSeismic::domain::EclipseGrid>()));
        }
    }
    d->updateWidget();
}

bool RegularGridImportPage::isComplete() const
{
    Q_D(const RegularGridImportPage);
    return d->m_ui->fileTreeWidget->topLevelItemCount() != 0
            && d->m_eclipseFilesImported
            && !qFuzzyIsNull(d->m_ui->regularGridXDimensionDoubleSpinBox->value())
            && !qFuzzyIsNull(d->m_ui->regularGridYDimensionDoubleSpinBox->value())
            && !qFuzzyIsNull(d->m_ui->regularGridZDimensionDoubleSpinBox->value());
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
