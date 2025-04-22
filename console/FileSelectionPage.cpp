#include <QTreeWidgetItem>
#include <QFileDialog>
#include "ui_FileSelectionPage.h"
#include "FileSelectionPage.h"

namespace syntheticSeismic {
namespace widgets {

namespace {
    const QLatin1Literal INPUT_FILES_FIELD ("input_files");

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

} // widgets
} // syntheticSeismic