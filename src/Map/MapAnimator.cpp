#include "MapAnimator.h"
#include "MapAnimator_P.h"

#include <OsmAndCore/QtExtensions.h>
#include <QtMath>
#include <QMutableListIterator>

#include <IMapRenderer.h>
#include <Logging.h>
#include <Utilities.h>

OsmAnd::MapAnimator::MapAnimator()
    : _p(new MapAnimator_P(this))
    , mapRenderer(_p->_renderer)
{
}

OsmAnd::MapAnimator::~MapAnimator()
{
}

void OsmAnd::MapAnimator::setMapRenderer(const std::shared_ptr<IMapRenderer>& mapRenderer)
{
    _p->setMapRenderer(mapRenderer);
}

bool OsmAnd::MapAnimator::isAnimationPaused() const
{
    return _p->isAnimationPaused();
}

bool OsmAnd::MapAnimator::isAnimationRunning() const
{
    return _p->isAnimationRunning();
}

void OsmAnd::MapAnimator::pauseAnimation()
{
    _p->pauseAnimation();
}

void OsmAnd::MapAnimator::resumeAnimation()
{
    _p->resumeAnimation();
}

void OsmAnd::MapAnimator::cancelAnimation()
{
    _p->cancelAnimation();
}

void OsmAnd::MapAnimator::update(const float timePassed)
{
    _p->update(timePassed);
}

void OsmAnd::MapAnimator::animateZoomBy(const float deltaValue, const float duration, const MapAnimatorTimingFunction timingFunction)
{
    _p->animateZoomBy(deltaValue, duration, timingFunction);
}

void OsmAnd::MapAnimator::animateZoomWith(const float velocity, const float deceleration)
{
    _p->animateZoomWith(velocity, deceleration);
}

void OsmAnd::MapAnimator::animateTargetBy(const PointI& deltaValue, const float duration, const MapAnimatorTimingFunction timingFunction)
{
    _p->animateTargetBy(deltaValue, duration, timingFunction);
}

void OsmAnd::MapAnimator::animateTargetBy(const PointI64& deltaValue, const float duration, const MapAnimatorTimingFunction timingFunction)
{
    _p->animateTargetBy(deltaValue, duration, timingFunction);
}

void OsmAnd::MapAnimator::animateTargetWith(const PointD& velocity, const PointD& deceleration)
{
    _p->animateTargetWith(velocity, deceleration);
}

void OsmAnd::MapAnimator::parabolicAnimateTargetBy(const PointI& deltaValue, const float duration, const MapAnimatorTimingFunction targetTimingFunction, const MapAnimatorTimingFunction zoomTimingFunction)
{
    _p->parabolicAnimateTargetBy(deltaValue, duration, targetTimingFunction, zoomTimingFunction);
}

void OsmAnd::MapAnimator::parabolicAnimateTargetBy(const PointI64& deltaValue, const float duration, const MapAnimatorTimingFunction targetTimingFunction, const MapAnimatorTimingFunction zoomTimingFunction)
{
    _p->parabolicAnimateTargetBy(deltaValue, duration, targetTimingFunction, zoomTimingFunction);
}

void OsmAnd::MapAnimator::parabolicAnimateTargetWith(const PointD& velocity, const PointD& deceleration)
{
    _p->parabolicAnimateTargetWith(velocity, deceleration);
}

void OsmAnd::MapAnimator::animateAzimuthBy(const float deltaValue, const float duration, const MapAnimatorTimingFunction timingFunction)
{
    _p->animateAzimuthBy(deltaValue, duration, timingFunction);
}

void OsmAnd::MapAnimator::animateAzimuthWith(const float velocity, const float deceleration)
{
    _p->animateAzimuthWith(velocity, deceleration);
}

void OsmAnd::MapAnimator::animateElevationAngleBy(const float deltaValue, const float duration, const MapAnimatorTimingFunction timingFunction)
{
    _p->animateElevationAngleBy(deltaValue, duration, timingFunction);
}

void OsmAnd::MapAnimator::animateElevationAngleWith(const float velocity, const float deceleration)
{
    _p->animateElevationAngleWith(velocity, deceleration);
}

void OsmAnd::MapAnimator::animateMoveBy(
    const PointI& deltaValue, const float duration,
    const bool zeroizeAzimuth, const bool invZeroizeElevationAngle,
    const MapAnimatorTimingFunction timingFunction)
{
    _p->animateMoveBy(deltaValue, duration, zeroizeAzimuth, invZeroizeElevationAngle, timingFunction);
}

void OsmAnd::MapAnimator::animateMoveBy(
    const PointI64& deltaValue, const float duration,
    const bool zeroizeAzimuth, const bool invZeroizeElevationAngle,
    const MapAnimatorTimingFunction timingFunction)
{
    _p->animateMoveBy(deltaValue, duration, zeroizeAzimuth, invZeroizeElevationAngle, timingFunction);
}

void OsmAnd::MapAnimator::animateMoveWith(const PointD& velocity, const PointD& deceleration, const bool zeroizeAzimuth, const bool invZeroizeElevationAngle)
{
    _p->animateMoveWith(velocity, deceleration, zeroizeAzimuth, invZeroizeElevationAngle);
}
