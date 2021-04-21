#include "EquationPlane.h"

using namespace syntheticSeismic::geometry;

namespace syntheticSeismic {
namespace domain {

EquationPlane::EquationPlane(const Point3D &p, const Point3D &q, const Point3D &r) : m_p(p), m_q(q), m_r(r)
{
    m_normalVector = crossProduct(m_p, m_q, m_r);
    m_termD = -(p.x * m_normalVector.x + q.y * m_normalVector.y + q.z * m_normalVector.z);
}

geometry::Point3D EquationPlane::crossProduct(const geometry::Point3D &p, const geometry::Point3D &q, const geometry::Point3D &r)
{
    const Point3D pq(q.x - p.x, q.y - p.y, q.z - p.z);
    const Point3D pr(r.x - p.x, r.y - p.y, r.z - p.z);

    return Point3D(pr.y * pq.z - pr.z * pq.y, pr.z * pq.x - pr.x * pq.z, pr.x * pq.y - pr.y * pq.x);
}

geometry::Point3D EquationPlane::getNormalVector() const
{
    return m_normalVector;
}

double EquationPlane::getTermD() const
{
    return m_termD;
}

double EquationPlane::calculateZByXY(double x, double y)
{
    return -(m_termD + m_normalVector.x * x + m_normalVector.y * y) / m_normalVector.z;
}

} // namespace domain
} // namespace syntheticSeis
