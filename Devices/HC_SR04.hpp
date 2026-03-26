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

namespace Device
{
    //Distance measurement
    template<InputCaptureConcept InputCapture_T, PwmConcept Pwm_T>
    class HC_SR04
    {
    private:
        InputCapture_T& echoRisingEdge;
        InputCapture_T& echoFallingEdge;
        Pwm_T& trigger;
        HAL::SoftwareTimer distanceMeasurementTimer{ 50 };

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

        HC_SR04(InputCapture_T& echoRisingEdge_, InputCapture_T& echoFallingEdge_, Pwm_T& trigger_)
            : echoRisingEdge(echoRisingEdge_)
            , echoFallingEdge(echoFallingEdge_)
            , trigger(trigger_)
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
