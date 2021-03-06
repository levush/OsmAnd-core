#include "Frustum2D.h"
#include "Utilities.h"

OsmAnd::Frustum2D::Frustum2D()
{
}

OsmAnd::Frustum2D::Frustum2D(const PointF& p0_, const PointF& p1_, const PointF& p2_, const PointF& p3_)
    : p0(p0_)
    , p1(p1_)
    , p2(p2_)
    , p3(p3_)
{
}

OsmAnd::Frustum2D::Frustum2D(const Frustum2D& that)
    : p0(that.p0)
    , p1(that.p1)
    , p2(that.p2)
    , p3(that.p3)
{
}

OsmAnd::Frustum2D::~Frustum2D()
{
}

OsmAnd::Frustum2D& OsmAnd::Frustum2D::operator=(const Frustum2D& that)
{
    p0 = that.p0;
    p1 = that.p1;
    p2 = that.p2;
    p3 = that.p3;

    return *this;
}

bool OsmAnd::Frustum2D::test(const PointF& p) const
{
    return isPointInside(p);
}

bool OsmAnd::Frustum2D::test(const PointF& lp0, const PointF& lp1) const
{
    // Check if any of line points is inside.
    // This case covers inner line and partially inner line (that has one vertex inside).
    if (isPointInside(lp0) || isPointInside(lp1))
        return true;

    // Check if line 'lp0-lp1' intersects any of edges.
    // This case covers intersecting line, that has start and stop outside of frustum.
    return
        testLineLineIntersection(lp0, lp1, p0, p1) ||
        testLineLineIntersection(lp0, lp1, p1, p2) ||
        testLineLineIntersection(lp0, lp1, p2, p3) ||
        testLineLineIntersection(lp0, lp1, p3, p0);
}

bool OsmAnd::Frustum2D::test(const AreaF& area) const
{
    const auto a0 = area.topLeft;
    const auto a1 = area.topRight();
    const auto a2 = area.bottomRight;
    const auto a3 = area.bottomLeft();

    // Check if any vertex of area is inside.
    // This case covers inner area and partially inner area (that has at least 1 vertex inside)
    if (isPointInside(a0) || isPointInside(a1) || isPointInside(a2) || isPointInside(a3))
        return true;

    // Check if any area edge intersects any of frustum edges.
    // This case covers intersecting area that has no vertex inside frustum.
    if (
        testLineLineIntersection(a0, a1, p0, p1) ||
        testLineLineIntersection(a0, a1, p1, p2) ||
        testLineLineIntersection(a0, a1, p2, p3) ||
        testLineLineIntersection(a0, a1, p3, p0) ||
        testLineLineIntersection(a1, a2, p0, p1) ||
        testLineLineIntersection(a1, a2, p1, p2) ||
        testLineLineIntersection(a1, a2, p2, p3) ||
        testLineLineIntersection(a1, a2, p3, p0) ||
        testLineLineIntersection(a2, a3, p0, p1) ||
        testLineLineIntersection(a2, a3, p1, p2) ||
        testLineLineIntersection(a2, a3, p2, p3) ||
        testLineLineIntersection(a2, a3, p3, p0) ||
        testLineLineIntersection(a3, a0, p0, p1) ||
        testLineLineIntersection(a3, a0, p1, p2) ||
        testLineLineIntersection(a3, a0, p2, p3) ||
        testLineLineIntersection(a3, a0, p3, p0))
        return true;

    // Check if frustum is totally inside area.
    return
        area.contains(p0) ||
        area.contains(p1) ||
        area.contains(p2) ||
        area.contains(p3);
}

bool OsmAnd::Frustum2D::isPointInside(const PointF& p) const
{
    // Check if point 'p' is on the same 'side' of each edge
    const auto sign0 = crossProductSign(p0, p1, p);
    const auto sign1 = crossProductSign(p1, p2, p);
    const auto sign2 = crossProductSign(p2, p3, p);
    const auto sign3 = crossProductSign(p3, p0, p);
    int sign = sign0;
    if (sign1 != 0)
    {
        if (sign != 0 && sign != sign1)
            return false;
        sign = sign1;
    }
    if (sign2 != 0)
    {
        if (sign != 0 && sign != sign2)
            return false;
        sign = sign2;
    }
    if (sign3 != 0)
    {
        if (sign != 0 && sign != sign3)
            return false;
    }
    return true;
}

