#include "EclipseGridView.h"

#include <domain/src/EclipseGrid.h>

#include <QPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScreen>

#include <QtDataVisualization/Q3DInputHandler>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QCustom3DVolume>
#include <QtDataVisualization/Q3DCamera>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/Q3DTheme>
#include <QtDataVisualization/qabstract3daxis.h>
#include <QtDataVisualization/Q3DInputHandler>

#include <QMessageBox>

using namespace QtDataVisualization;

namespace invertseis {
namespace widgets {

class EclipseGridViewPrivate
{
public:
    EclipseGridViewPrivate();

    void createPoints();

    void toggleAreaAll(bool enabled)
    {
        if (enabled) {
            m_graph->axisX()->setRange(0.0f, 1000.0f);
            m_graph->axisY()->setRange(-600.0f, 600.0f);
            m_graph->axisZ()->setRange(0.0f, 1000.0f);
            m_graph->axisX()->setSegmentCount(5);
            m_graph->axisY()->setSegmentCount(6);
            m_graph->axisZ()->setSegmentCount(5);
        }
    }

public:
    QPointer<invertseis::data::Entity> m_entity;
    //    QVector<> m_points;
    Q3DScatter *m_graph;
    QScatterDataProxy *m_dataProxy;
    QScatter3DSeries *m_series;


};


EclipseGridViewPrivate::EclipseGridViewPrivate()
    : m_entity()
    , m_graph(new Q3DScatter())
    , m_dataProxy(nullptr)
    , m_series(nullptr)
{
}

/**********************************************************************/

EclipseGridView::EclipseGridView(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new EclipseGridViewPrivate())
{
    Q_D(EclipseGridView);
    QWidget *scatterWidgetCntainer = QWidget::createWindowContainer(d->m_graph);

    if (!d_ptr->m_graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        d_ptr->m_graph->deleteLater();
        scatterWidgetCntainer->deleteLater();
    }
    else{
        QSize screenSize = d_ptr->m_graph->screen()->size();
        scatterWidgetCntainer->setMinimumSize(QSize(screenSize.width() / 3, screenSize.height() / 3));
        scatterWidgetCntainer->setMaximumSize(screenSize);
        scatterWidgetCntainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        scatterWidgetCntainer->setFocusPolicy(Qt::StrongFocus);

        QHBoxLayout *hLayout = new QHBoxLayout(this);
        QVBoxLayout *vLayout = new QVBoxLayout();
        QVBoxLayout *vLayout2 = new QVBoxLayout();
        hLayout->addWidget(scatterWidgetCntainer, 1);
        hLayout->addLayout(vLayout);
        hLayout->addLayout(vLayout2);

        this->setWindowTitle(QStringLiteral("Volumetric object example - 3D terrain"));

        d_ptr->m_graph->activeTheme()->setType(Q3DTheme::ThemeQt);
        d_ptr->m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
        d_ptr->m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);
        d_ptr->m_graph->activeTheme()->setBackgroundEnabled(false);

        QCustom3DVolume* m_volumeItem = new QCustom3DVolume;
        // Adjust water level to zero with a minor tweak to y-coordinate position and scaling
        m_volumeItem->setScaling(
                    QVector3D(d->m_graph->axisX()->max() - d->m_graph->axisX()->min(),
                              (d->m_graph->axisY()->max() - d->m_graph->axisY()->min()) * 0.91f,
                              d->m_graph->axisZ()->max() - d->m_graph->axisZ()->min()));
        Q_D(EclipseGridView);
        m_volumeItem->setPosition(
                    QVector3D((d->m_graph->axisX()->max() + d->m_graph->axisX()->min()) / 2.0f,
                              -0.045f * (d->m_graph->axisY()->max() - d->m_graph->axisY()->min()) + (d->m_graph->axisY()->max() + d->m_graph->axisY()->min()) / 2.0f,
                              (d->m_graph->axisZ()->max() + d->m_graph->axisZ()->min()) / 2.0f));
        m_volumeItem->setScalingAbsolute(false);

        d_ptr->m_graph->addCustomItem(m_volumeItem);

//        m_volumeItem->set
        //! [0]
        //! [1]
//        m_volumeItem->setTextureWidth(lowDetailSize);
//        m_volumeItem->setTextureHeight(lowDetailSize / 2);
//        m_volumeItem->setTextureDepth(lowDetailSize);
//        m_volumeItem->setTextureFormat(QImage::Format_Indexed8);
//        m_volumeItem->setTextureData(new QVector<uchar>(*m_lowDetailData));

    }
}

QPointer<data::Entity> EclipseGridView::eclipseGridEntity() const
{
    Q_D(const EclipseGridView);
    return d->m_entity;
}

bool EclipseGridView::setEclipseGridEntity(QPointer<data::Entity> entity)
{
    if(!entity){
        return false;
    }

    std::shared_ptr<data::DomainObject> domainObject = entity->domainObject();
    domain::EclipseGrid* e = dynamic_cast<domain::EclipseGrid*>(domainObject.get());

    auto eclipseGrid = std::dynamic_pointer_cast<invertseis::domain::EclipseGrid>(domainObject);
    if(!eclipseGrid){
        return false;
    }

    Q_D(EclipseGridView);
    d->m_entity = entity;

    d->m_graph->axisX()->setTitle("X");
    d->m_graph->axisY()->setTitle("Y");
    d->m_graph->axisZ()->setTitle("Z");

    d->m_graph->axisX()->setTitleVisible(true);
    d->m_graph->axisY()->setTitleVisible(true);
    d->m_graph->axisZ()->setTitleVisible(true);

    if(!(d->m_series && d->m_dataProxy)){
        d->m_dataProxy = new QScatterDataProxy;
        d->m_series = new QScatter3DSeries(d->m_dataProxy);
        d->m_series->setItemSize(d->m_series->itemSize()* 0.01);
        d->m_series->setMeshSmooth(true);
        d->m_series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
        d->m_graph->addSeries(d->m_series);
    }

    QScatterDataArray *dataArray = new QScatterDataArray;
    dataArray->resize(eclipseGrid->coordinates().size());

    QScatterDataItem *ptrToDataArray = &dataArray->first();

    for(const invertseis::geometry::Coordinate& coord : eclipseGrid->coordinates()){
        ptrToDataArray->setPosition(QVector3D(coord.x(), coord.y(), coord.z()));
        ++ptrToDataArray;
    }

    d->m_dataProxy->resetArray(dataArray);
    return true;
}

} // namespace widgets
} // namespace invertseis


