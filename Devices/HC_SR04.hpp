#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>
#include <optional>

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
    protected:

        enum class State
        {
            Idle,
            WaitingForRisingEdge,
            WaitingForFallingEdge,
        };

        InputCapture_T& echoRisingEdge;
        InputCapture_T& echoFallingEdge;
        Pwm_T& trigger;
        SoftwareTimer_T distanceMeasurementTimer{};

        State state{ State::Idle };

        volatile uint32_t start{ 0 };
        volatile uint32_t stop{ 0 };

        // returns air sound velocity in [m/s]
        float CalculateAirSoundVelocity(const float tempC) const
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

        HC_SR04(InputCapture_T& echoRisingEdge_, InputCapture_T& echoFallingEdge_, Pwm_T& trigger_, const SoftwareTimer_T& distanceMeasurementTimer_)
            : echoRisingEdge(echoRisingEdge_)
            , echoFallingEdge(echoFallingEdge_)
            , trigger(trigger_)
            , distanceMeasurementTimer(distanceMeasurementTimer_)
        {
        };

        void Trigger()
        {
            trigger.ResetCounter();
            trigger.Start();
            while (trigger.GetCounter() < trigger.GetMaxCounter())
            {};//potentially add timeout
            trigger.Stop();
        }

        std::optional<float> GetDistance(const float tempC = 20.f)
        {
            switch (state)
            {
            case State::Idle:
                if (distanceMeasurementTimer.IsExpired())
                {
                    Trigger();
                    distanceMeasurementTimer.Reset();
                    state = State::WaitingForRisingEdge;
                }
                break;

            case State::WaitingForRisingEdge:
                if (distanceMeasurementTimer.IsExpired())
                {
                    state = State::Idle;
                    break;
                }

                if (const auto readValue = echoRisingEdge.Read(); readValue != 0)
                {   
                    start = readValue;
                    state = State::WaitingForFallingEdge;
                }

                break;
            case State::WaitingForFallingEdge:
                if (distanceMeasurementTimer.IsExpired())
                {
                    state = State::Idle;
                    break;
                }
                
                if (const auto readValue = echoFallingEdge.Read(); readValue != 0)
                {
                    stop = readValue;
                    state = State::Idle;

                    const auto deltaTime = (stop >= start) ? (stop - start) : (echoRisingEdge.GetMaxCounter() - start + stop + 1); // handle timer overflow

                    static constexpr unsigned us_in_s = 1'000'000;
                    static constexpr unsigned cm_in_m = 100;
                    return deltaTime * (CalculateAirSoundVelocity(tempC) / (2 * us_in_s)) * cm_in_m;
                }
                break;
            }

            return std::nullopt;
        };
    };
};
