#pragma once

#include <QObject>
#include <QString>
#include <memory>
#include "domain/src/RegularGrid.h"

#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPlane.h>
#include <vtkImageData.h>

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
                               const QString& scalarBarTitle = QStringLiteral("Value"),
                               bool useNearestInterpolation = false);
    ~RegularGridWorker();

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> getRenderWindow() const { return m_renderWindow; }
    vtkSmartPointer<vtkPlane> getClipPlane() const { return m_clipPlane; }
    // Full (un-trimmed) ImageData. Returned for the dialog's "trim empty
    // slices" checkbox: the slice mappers are attached to the trimmed
    // ImageData by default, and the dialog swaps to this one when the user
    // unticks the checkbox to see the full volume including the no-data tail.
    vtkSmartPointer<vtkImageData> getFullImageData() const { return m_imageDataFull; }

public slots:
    void run();

private:
    void buildGrid();

    struct FillResult {
        double minValue;
        double maxValue;
    };
    // Populates imageData's scalars from grid cells (parallel), computing
    // min/max over non-noData cells. Emits stepProgress within [progressLo,
    // progressHi]. imageData must already have dimensions / origin / spacing
    // / allocation set up.
    FillResult fillImageDataFromGrid(const domain::RegularGrid<double>& grid,
                                     vtkImageData* imageData,
                                     int progressLo, int progressHi);

    // Returns the effective number of Z cells that actually contain data.
    // Walks backward from k=dimZ-1 dropping XY slices that are either
    // all-noData or uniform (fuzzy compare, since values are doubles).
    static size_t computeEffectiveDimZ(const domain::RegularGrid<double>& grid);

    // Returns a new grid that is a copy of src with only its first
    // newNumberOfCellsInZ Z cells. All other metadata (cell sizes, rectangle,
    // z top/bottom, noData sentinel, etc.) is preserved verbatim.
    static std::shared_ptr<domain::RegularGrid<double>> copyTrimmedZ(
        const domain::RegularGrid<double>& src, size_t newNumberOfCellsInZ);

private:
    const std::shared_ptr<domain::RegularGrid<double>> m_regularGrid;
    const QString m_scalarBarTitle;
    const bool m_useNearestInterpolation;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkPlane> m_clipPlane;
    vtkSmartPointer<vtkImageData> m_imageDataFull;

    const int m_totalSteps = 100;
    std::atomic<int> m_currentSteps{0};
};

}
}
