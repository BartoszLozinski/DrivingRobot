#pragma once

#include <type_traits>
#include <concepts>

#include <cstdint>

template<typename T>
concept InputCaptureConcept = requires (T inputCapture)
{
    inputCapture.Read();
};

template<typename T>
concept PwmConcept = requires (T pwm, const uint32_t pulse)
{
    pwm.Start();
    pwm.Stop();
};

namespace Device
{
    template<InputCaptureConcept InputCapture_T/*, PwmConcept Pwm_T*/>
    class HC_SR04
    {
    private:
        InputCapture_T& echoRisingEdge;
        InputCapture_T& echoFallingEdge;
        //Pwm_T& trigger; //todo add getState to PWM
        // and start pwm if it's in reset state
        
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

        HC_SR04(InputCapture_T& echoRisingEdge_, InputCapture_T& echoFallingEdge_)
            : echoRisingEdge(echoRisingEdge_)
            , echoFallingEdge(echoFallingEdge_)
        {};

        // return distance in [cm]
        float GetDistance(const float tempC = 20.f)
        {
            static constexpr unsigned us_in_s = 1'000'000;
            const auto start = echoRisingEdge.Read();
            const auto stop = echoFallingEdge.Read();
            return (stop -start) * (CalculateAirSoundVelocity(tempC) / (2 * us_in_s)) * 100;
        }
    };
};
