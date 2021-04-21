#pragma once
#include "../../geometry/src/Point3D.h"

namespace syntheticSeismic {

namespace domain {

class EquationPlane
{
public:
    EquationPlane(const geometry::Point3D &p, const geometry::Point3D &q, const geometry::Point3D &r);

    geometry::Point3D getNormalVector() const;

    double getTermD() const;

    double calculateZByXY(double x, double y);

    static geometry::Point3D crossProduct(const geometry::Point3D &p, const geometry::Point3D &q, const geometry::Point3D &r);
private:
    geometry::Point3D m_p;
    geometry::Point3D m_q;
    geometry::Point3D m_r;

    geometry::Point3D m_normalVector;
    double m_termD;
};

} // namespace domain
} // namespace syntheticSeis
