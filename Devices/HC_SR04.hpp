#pragma once

#include <type_traits>
#include <concepts>
#include <cstdint>

#include "../Peripherals/Timer/IPwm.hpp"
#include "../Peripherals/Timer/HAL/SoftwareTimer.hpp"

template<typename T>
concept InputCaptureConcept = requires (T inputCapture)
{
    inputCapture.Read();
};

template<typename T>
concept PwmConcept = requires (T pwm, const uint32_t pulse)
{
    pwm.Start();
    pwm.GetState();
    requires std::convertible_to<decltype(pwm.GetState()), Peripherals::PwmState>;
};

template<typename T>
concept SoftwareTimerConcept = 
    std::constructible_from<T, uint32_t> &&
    requires (T timer, uint32_t period)
{
    { timer.IsExpired() } -> std::same_as<bool>;
    { timer.Reset() } -> std::same_as<void>;
    { timer.Now() } -> std::same_as<uint32_t>; 
};

namespace Device
{
    //Distance measurement
    template<InputCaptureConcept InputCapture_T, PwmConcept Pwm_T, SoftwareTimerConcept SoftwareTimer_T>
    class HC_SR04
    {
    private:
        InputCapture_T& echoRisingEdge;
        InputCapture_T& echoFallingEdge;
        Pwm_T& trigger;
        SoftwareTimer_T distanceMeasurementTimer{};

        // returns air sound velocity in [m/s]
        float CalculateAirSoundVelocity(const float tempC)
        {
            return 331.8f + 0.6f * tempC;
        }

        float MeasureDeltaTime()
        {
            distanceMeasurementTimer.Reset();
            uint32_t start_prev = echoRisingEdge.Read();
            uint32_t start = start_prev;
            while (start == start_prev && !distanceMeasurementTimer.IsExpired())
                start = echoRisingEdge.Read();

            distanceMeasurementTimer.Reset();
            uint32_t stop_prev = echoFallingEdge.Read();
            uint32_t stop = stop_prev;
            while (stop == stop_prev && !distanceMeasurementTimer.IsExpired())
                stop = echoFallingEdge.Read();
            
            return stop - start;
        }

    public:

        HC_SR04() = delete;
        HC_SR04(const HC_SR04&) = delete;
        HC_SR04(HC_SR04&&) = delete;
        HC_SR04& operator=(const HC_SR04&) = delete;
        HC_SR04& operator=(HC_SR04&&) = delete;
        ~HC_SR04() = default;

        HC_SR04(InputCapture_T& echoRisingEdge_, InputCapture_T& echoFallingEdge_, Pwm_T& trigger_, const SoftwareTimer_T& distanceMeasurementTimer_)
            : echoRisingEdge(echoRisingEdge_)
            , echoFallingEdge(echoFallingEdge_)
            , trigger(trigger_)
            , distanceMeasurementTimer(distanceMeasurementTimer_)
        {
        };

        void Trigger() const
        {
            if (trigger.GetState() == Peripherals::PwmState::READY)
                trigger.Start();
        }

        // return distance in [cm]
        float GetDistance(const float tempC = 20.f)
        {
            static constexpr unsigned us_in_s = 1'000'000;
            return MeasureDeltaTime() * (CalculateAirSoundVelocity(tempC) / (2 * us_in_s)) * 100;
        }
    };
};
