#pragma once

#include <type_traits>
#include <concepts>
#include <cstdint>

#include "../Peripherals/Timer/IPwm.hpp"

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
        
        // returns air sound velocity in [m/s]
        float CalculateAirSoundVelocity(const float tempC)
        {
            return 331.8f + 0.6f * tempC;
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
            if (trigger.GetState() == Peripherals::PwmState::READY)
                trigger.Start();
        };

        void Trigger() const
        {
            trigger.ResetCounter();
            trigger.Start();
        }

        // return distance in [cm]
        float GetDistance(const uint32_t start, const uint32_t stop, const float tempC = 20.f)
        {
            const uint32_t timerPeriod = 0xFFFF;
            uint32_t delta;

            if (stop >= start)
                delta = stop - start;
            else
                delta = (timerPeriod - start) + stop + 1;

            static constexpr unsigned us_in_s = 1'000'000;
            return delta * (CalculateAirSoundVelocity(tempC) / (2 * us_in_s)) * 100;
        }
    };
};
