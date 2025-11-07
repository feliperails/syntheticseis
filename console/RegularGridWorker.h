#pragma once

#include <QObject>
#include <memory>
#include "domain/src/RegularGrid.h"

#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>

using namespace syntheticSeismic;
using namespace syntheticSeismic::geometry;

namespace syntheticSeismic {
namespace widgets {


class RegularGridWorker : public QObject
{
    Q_OBJECT

signals:
    void stepProgress(int currentStep);
    void finished();

public:
    static constexpr double M_ZOOM_FACTOR_Z = 0.1;

    explicit RegularGridWorker(const std::shared_ptr<domain::RegularGrid<double>>& regularGrid);
    ~RegularGridWorker();

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> getRenderWindow() const { return m_renderWindow; }

public slots:
    void run();

private:
    void buildGrid();

private:
    const std::shared_ptr<domain::RegularGrid<double>> m_regularGrid;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;

    const int m_totalSteps = 100;
    std::atomic<int> m_currentSteps{0};
};

}
}
