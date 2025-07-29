#pragma once

#include <QObject>
#include <memory>
#include "geometry/src/Volume.h"

#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>

using namespace syntheticSeismic;
using namespace syntheticSeismic::geometry;

namespace syntheticSeismic {
namespace widgets {


class EclipseGridWorker : public QObject
{
    Q_OBJECT

signals:
    void stepProgress(int currentStep);
    void finished();

public:
    explicit EclipseGridWorker(const std::vector<std::shared_ptr<Volume>> * const & allVolumes);
    ~EclipseGridWorker();

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> getRenderWindow() const { return m_renderWindow; }

public slots:
    void run();

private:
    void init();
    void buildGrid();

private:

    const std::vector<std::shared_ptr<Volume>>* m_allVolumes;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;

    std::vector<std::array<double, 3>> m_defaultColors;

    int m_currentSteps = 0;
    const int m_totalSteps = 100;
};

}
}
