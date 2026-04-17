#pragma once

#include <QObject>
#include <QString>
#include <memory>
#include "domain/src/RegularGrid.h"

#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPlane.h>

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
    static constexpr double M_ZOOM_FACTOR_Z = 1.0;

    explicit RegularGridWorker(const std::shared_ptr<domain::RegularGrid<double>>& regularGrid,
                               const QString& scalarBarTitle = QStringLiteral("Value"));
    ~RegularGridWorker();

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> getRenderWindow() const { return m_renderWindow; }
    vtkSmartPointer<vtkPlane> getClipPlane() const { return m_clipPlane; }

public slots:
    void run();

private:
    void buildGrid();

private:
    const std::shared_ptr<domain::RegularGrid<double>> m_regularGrid;
    const QString m_scalarBarTitle;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkPlane> m_clipPlane;

    const int m_totalSteps = 100;
    std::atomic<int> m_currentSteps{0};
};

}
}
