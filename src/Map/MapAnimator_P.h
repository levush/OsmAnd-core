#ifndef _OSMAND_CORE_MAP_ANIMATOR_P_H_
#define _OSMAND_CORE_MAP_ANIMATOR_P_H_

#include "stdlib_common.h"
#include <cassert>
#include <functional>

#include "QtExtensions.h"
#include <QtMath>
#include <QHash>
#include <QList>
#include <QVector>
#include <QMutex>
#include <QVariant>

#include "OsmAndCore.h"
#include "PrivateImplementation.h"
#include "MapTypes.h"

namespace OsmAnd
{
    class IMapRenderer;

    class MapAnimator;
    class MapAnimator_P
    {
        Q_DISABLE_COPY(MapAnimator_P);
    private:
    protected:
        MapAnimator_P(MapAnimator* const owner);

        ImplementationInterface<MapAnimator> owner;
        std::shared_ptr<IMapRenderer> _renderer;

        struct AnimationContext
        {
            QVariantList storageList;
            QVariantHash storageHash;
        };

        class BaseAnimation
        {
            Q_DISABLE_COPY(BaseAnimation);
        private:
        protected:
            float _timePassed;

            AnimationContext _ownContext;
            const std::shared_ptr<AnimationContext> _sharedContext;
        
            BaseAnimation(const float duration_, const float delay_, const MapAnimatorTimingFunction timingFunction_, const std::shared_ptr<AnimationContext>& sharedContext_)
                : _timePassed(0.0f)
                , _sharedContext(sharedContext_)
                , duration(duration_)
                , delay(delay_)
                , timingFunction(timingFunction_)
            {
            }

            static float properCast(const float value)
            {
                return value;
            }

            static double properCast(const double value)
            {
                return value;
            }

            static double properCast(const int32_t value)
            {
                return static_cast<double>(value);
            }

            static double properCast(const int64_t value)
            {
                return static_cast<double>(value);
            }

            template <typename T>
            static void calculateValue(const float t, const T initial, const T delta, const float duration, const MapAnimatorTimingFunction timingFunction, T& value)
            {
                switch(timingFunction)
                {
                case MapAnimatorTimingFunction::Linear:
                    value = initial + static_cast<T>(linearTween(t, properCast(delta), duration));
                    break;

#define _DECLARE_USE(name)                                                                                                          \
    case MapAnimatorTimingFunction::EaseIn##name:                                                                                   \
        value = initial + static_cast<T>(easeIn_##name(t, properCast(delta), duration));                                            \
        break;                                                                                                                      \
    case MapAnimatorTimingFunction::EaseOut##name:                                                                                  \
        value = initial + static_cast<T>(easeOut_##name(t, properCast(delta), duration));                                           \
        break;                                                                                                                      \
    case MapAnimatorTimingFunction::EaseInOut##name:                                                                                \
        value = initial + static_cast<T>(easeInOut_##name(t, properCast(delta), duration));                                         \
        break;                                                                                                                      \
    case MapAnimatorTimingFunction::EaseOutIn##name:                                                                                \
        value = initial + static_cast<T>(easeOutIn_##name(t, properCast(delta), duration));                                         \
        break;

                _DECLARE_USE(Quadratic)
                _DECLARE_USE(Cubic)
                _DECLARE_USE(Quartic)
                _DECLARE_USE(Quintic)
                _DECLARE_USE(Sinusoidal)
                _DECLARE_USE(Exponential)
                _DECLARE_USE(Circular)

#undef _DECLARE_USE

                case MapAnimatorTimingFunction::Invalid:
                default:
                    assert(false);
                    break;
                }
            }

            template <typename T>
            static void calculateValue(const float t, const Point<T>& initial, const Point<T>& delta, const float duration, const MapAnimatorTimingFunction timingFunction, Point<T>& value)
            {
                calculateValue(t, initial.x, delta.x, duration, timingFunction, value.x);
                calculateValue(t, initial.y, delta.y, duration, timingFunction, value.y);
            }

            template <typename T>
            static T linearTween(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return nt*delta;
            }

#define _DECLARE_IN_OUT_AND_OUT_IN(name)                                                                                                    \
    template <typename T>                                                                                                                   \
    static T easeInOut_##name(const float t, const T delta, const float duration)                                                           \
    {                                                                                                                                       \
        const auto halfDuration = 0.5f * duration;                                                                                          \
        const auto halfDelta = 0.5f * delta;                                                                                                \
        const auto tn = t / halfDuration;                                                                                                   \
        if (tn < 1.0f)                                                                                                                       \
            return easeIn_##name(tn * halfDuration, halfDelta, halfDuration);                                                               \
        else                                                                                                                                \
            return halfDelta + easeOut_##name((tn - 1.0f) * halfDuration, halfDelta, halfDuration);                                         \
    }                                                                                                                                       \
    template <typename T>                                                                                                                   \
    static T easeOutIn_##name(const float t, const T delta, const float duration)                                                           \
    {                                                                                                                                       \
        const auto halfDuration = 0.5f * duration;                                                                                          \
        const auto halfDelta = 0.5f * delta;                                                                                                \
        const auto tn = t / halfDuration;                                                                                                   \
        if (tn < 1.0f)                                                                                                                       \
            return easeOut_##name(tn * halfDuration, halfDelta, halfDuration);                                                              \
        else                                                                                                                                \
            return halfDelta + easeIn_##name((tn - 1.0f) * halfDuration, halfDelta, halfDuration);                                          \
    }

            template <typename T>
            static T easeIn_Quadratic(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return (nt*nt)*delta;
            }
            template <typename T>
            static T easeOut_Quadratic(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return -nt*(nt-2.0f)*delta;
            }
            _DECLARE_IN_OUT_AND_OUT_IN(Quadratic);

            template <typename T>
            static T easeIn_Cubic(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return nt*nt*nt*delta;
            }
            template <typename T>
            static T easeOut_Cubic(const float t, const T delta, const float duration)
            {
                auto nt = t / duration;
                nt -= 1.0f;
                return (nt*nt*nt + 1.0f)*delta;
            }
            _DECLARE_IN_OUT_AND_OUT_IN(Cubic);

            template <typename T>
            static T easeIn_Quartic(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return nt*nt*nt*nt*delta;
            }
            template <typename T>
            static T easeOut_Quartic(const float t, const T delta, const float duration)
            {
                auto nt = t / duration;
                nt -= 1.0f;
                return -(nt*nt*nt*nt - 1.0f)*delta;
            }
            _DECLARE_IN_OUT_AND_OUT_IN(Quartic);

            template <typename T>
            static T easeIn_Quintic(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return nt*nt*nt*nt*nt*delta;
            }
            template <typename T>
            static T easeOut_Quintic(const float t, const T delta, const float duration)
            {
                auto nt = t / duration;
                nt -= 1.0f;
                return (nt*nt*nt*nt*nt + 1.0f)*delta;
            }
            _DECLARE_IN_OUT_AND_OUT_IN(Quintic);

            template <typename T>
            static T easeIn_Sinusoidal(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return delta - qCos(nt * M_PI_2)*delta;
            }
            template <typename T>
            static T easeOut_Sinusoidal(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return qSin(nt * M_PI_2)*delta;
            }
            _DECLARE_IN_OUT_AND_OUT_IN(Sinusoidal);

            template <typename T>
            static T easeIn_Exponential(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return qPow( 2.0f, 10.0f * (nt - 1.0f) )*delta;
            }
            template <typename T>
            static T easeOut_Exponential(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return ( -qPow( 2.0f, -10.0f * nt ) + 1.0f )*delta;
            }
            _DECLARE_IN_OUT_AND_OUT_IN(Exponential);

            template <typename T>
            static T easeIn_Circular(const float t, const T delta, const float duration)
            {
                const auto nt = t / duration;
                return -(qSqrt(1.0f - nt*nt) - 1.0f)*delta;
            }
            template <typename T>
            static T easeOut_Circular(const float t, const T delta, const float duration)
            {
                auto nt = t / duration;
                nt -= 1.0f;
                return qSqrt(1.0f - nt*nt)*delta;
            }
            _DECLARE_IN_OUT_AND_OUT_IN(Circular);

#undef _DECLARE_IN_OUT
        public:
            virtual ~BaseAnimation()
            {
            }

            virtual bool process(const float timePassed) = 0;

            const float delay;
            const float duration;

            const MapAnimatorTimingFunction timingFunction;
        };

        template <typename T>
        class Animation : public BaseAnimation
        {
            Q_DISABLE_COPY(Animation);
        public:
            typedef std::function<void (const T newValue, AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext)> ApplierMethod;
            typedef std::function<T (AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext)> GetInitialValueMethod;
            typedef std::function<T (AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext)> GetDeltaValueMethod;
        private:
        protected:
            T _initialValue;
            bool _initialValueCaptured;
            T _deltaValue;
        public:
            Animation(
                const T deltaValue_,
                const float duration_,
                const float delay_,
                const MapAnimatorTimingFunction timingFunction_,
                const GetInitialValueMethod obtainer_,
                const ApplierMethod applier_,
                const std::shared_ptr<AnimationContext>& sharedContext_ = nullptr)
                : BaseAnimation(duration_, delay_, timingFunction_, sharedContext_)
                , _initialValueCaptured(false)
                , _deltaValue(deltaValue_)
                , deltaValue(_deltaValue)
                , deltaValueObtainer(nullptr)
                , obtainer(obtainer_)
                , applier(applier_)
            {
                assert(obtainer != nullptr);
                assert(applier != nullptr);
            }

            Animation(
                const GetDeltaValueMethod deltaValueObtainer_,
                const float duration_,
                const float delay_,
                const MapAnimatorTimingFunction timingFunction_,
                const GetInitialValueMethod obtainer_,
                const ApplierMethod applier_,
                const std::shared_ptr<AnimationContext>& sharedContext_ = nullptr)
                : BaseAnimation(duration_, delay_, timingFunction_, sharedContext_)
                , _initialValueCaptured(false)
                , deltaValue(_deltaValue)
                , deltaValueObtainer(deltaValueObtainer_)
                , obtainer(obtainer_)
                , applier(applier_)
            {
                assert(obtainer != nullptr);
                assert(applier != nullptr);
            }

            virtual ~Animation()
            {
            }

            virtual bool process(const float timePassed)
            {
                // Increment time
                _timePassed += timePassed;

                // Check for delay
                if (_timePassed < delay)
                    return false;

                // If this is first frame, and initial value has not been captured, do that
                if (!_initialValueCaptured)
                {
                    _initialValue = obtainer(_ownContext, _sharedContext);
                    _initialValueCaptured = true;
                    if (deltaValueObtainer)
                        _deltaValue = deltaValueObtainer(_ownContext, _sharedContext);

                    return false;
                }

                // Calculate time
                const auto currentTime = qMin(_timePassed - delay, duration);
               
                // Obtain current delta
                T newValue;
                calculateValue(currentTime, _initialValue, _deltaValue, duration, timingFunction, newValue);

                // Apply new value
                applier(newValue, _ownContext, _sharedContext);

                // Return false to indicate that processing has not yet finished
                return ((_timePassed - delay) >= duration);
            }

            const T& deltaValue;
            const GetDeltaValueMethod deltaValueObtainer;
            const GetInitialValueMethod obtainer;
            const ApplierMethod applier;
        };

        volatile bool _isAnimationPaused;
        mutable QMutex _animationsMutex;
        QList< std::shared_ptr<BaseAnimation> > _animations;

        void constructZoomAnimation(
            QList< std::shared_ptr<BaseAnimation> >& outAnimation,
            const float deltaValue,
            const float duration,
            const MapAnimatorTimingFunction timingFunction);
        void constructTargetAnimation(
            QList< std::shared_ptr<BaseAnimation> >& outAnimation,
            const PointI64& deltaValue,
            const float duration,
            const MapAnimatorTimingFunction timingFunction);
        void constructParabolicTargetAnimation(
            QList< std::shared_ptr<BaseAnimation> >& outAnimation,
            const PointI64& deltaValue,
            const float duration,
            const MapAnimatorTimingFunction targetTimingFunction,
            const MapAnimatorTimingFunction zoomTimingFunction);
        void constructAzimuthAnimation(
            QList< std::shared_ptr<BaseAnimation> >& outAnimation,
            const float deltaValue,
            const float duration,
            const MapAnimatorTimingFunction timingFunction);
        void constructElevationAngleAnimation(
            QList< std::shared_ptr<BaseAnimation> >& outAnimation,
            const float deltaValue,
            const float duration,
            const MapAnimatorTimingFunction timingFunction);
        void constructZeroizeAzimuthAnimation(
            QList< std::shared_ptr<BaseAnimation> >& outAnimation,
            const float duration,
            const MapAnimatorTimingFunction timingFunction);
        void constructInvZeroizeElevationAngleAnimation(
            QList< std::shared_ptr<BaseAnimation> >& outAnimation,
            const float duration,
            const MapAnimatorTimingFunction timingFunction);

        const Animation<float>::GetInitialValueMethod _zoomGetter;
        float zoomGetter(AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext);
        const Animation<float>::ApplierMethod _zoomSetter;
        void zoomSetter(const float newValue, AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext);

        const Animation<float>::GetInitialValueMethod _azimuthGetter;
        float azimuthGetter(AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext);
        const Animation<float>::ApplierMethod _azimuthSetter;
        void azimuthSetter(const float newValue, AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext);

        const Animation<float>::GetInitialValueMethod _elevationAngleGetter;
        float elevationAngleGetter(AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext);
        const Animation<float>::ApplierMethod _elevationAngleSetter;
        void elevationAngleSetter(const float newValue, AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext);

        const Animation<PointI64>::GetInitialValueMethod _targetGetter;
        PointI64 targetGetter(AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext);
        const Animation<PointI64>::ApplierMethod _targetSetter;
        void targetSetter(const PointI64 newValue, AnimationContext& context, const std::shared_ptr<AnimationContext>& sharedContext);
    public:
        ~MapAnimator_P();

        bool isAnimationPaused() const;
        bool isAnimationRunning() const;

        void pauseAnimation();
        void resumeAnimation();
        void cancelAnimation();

        void setMapRenderer(const std::shared_ptr<IMapRenderer>& mapRenderer);

        void update(const float timePassed);

        void animateZoomBy(const float deltaValue, const float duration, const MapAnimatorTimingFunction timingFunction);
        void animateZoomWith(const float velocity, const float deceleration);

        void animateTargetBy(const PointI& deltaValue, const float duration, const MapAnimatorTimingFunction timingFunction);
        void animateTargetBy(const PointI64& deltaValue, const float duration, const MapAnimatorTimingFunction timingFunction);
        void animateTargetWith(const PointD& velocity, const PointD& deceleration);

        void parabolicAnimateTargetBy(const PointI& deltaValue, const float duration, const MapAnimatorTimingFunction targetTimingFunction, const MapAnimatorTimingFunction zoomTimingFunction);
        void parabolicAnimateTargetBy(const PointI64& deltaValue, const float duration, const MapAnimatorTimingFunction targetTimingFunction, const MapAnimatorTimingFunction zoomTimingFunction);
        void parabolicAnimateTargetWith(const PointD& velocity, const PointD& deceleration);

        void animateAzimuthBy(const float deltaValue, const float duration, const MapAnimatorTimingFunction timingFunction);
        void animateAzimuthWith(const float velocity, const float deceleration);

        void animateElevationAngleBy(const float deltaValue, const float duration, const MapAnimatorTimingFunction timingFunction);
        void animateElevationAngleWith(const float velocity, const float deceleration);

        void animateMoveBy(
            const PointI& deltaValue, const float duration,
            const bool zeroizeAzimuth, const bool invZeroizeElevationAngle,
            const MapAnimatorTimingFunction timingFunction);
        void animateMoveBy(
            const PointI64& deltaValue, const float duration,
            const bool zeroizeAzimuth, const bool invZeroizeElevationAngle,
            const MapAnimatorTimingFunction timingFunction);
        void animateMoveWith(const PointD& velocity, const PointD& deceleration, const bool zeroizeAzimuth, const bool invZeroizeElevationAngle);

    friend class OsmAnd::MapAnimator;
    };
}

#endif // !defined(_OSMAND_CORE_MAP_ANIMATOR_P_H_)
