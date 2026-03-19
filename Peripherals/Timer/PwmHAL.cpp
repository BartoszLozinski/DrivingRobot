#include "PwmHAL.hpp"


namespace Peripherals
{
    PwmHAL::PwmHAL(TIM_HandleTypeDef& timer_, const uint32_t channel_)
        : timer(timer_)
        , channel(channel_)
    {};

    void PwmHAL::Start_Impl()
    {
        HAL_TIM_PWM_Start(&timer, channel);
    }

    void PwmHAL::Stop_Impl()
    {
        HAL_TIM_PWM_Stop(&timer, channel);
    };

    void PwmHAL::SetPulse_Impl(const uint32_t pulse)
    {
        const uint32_t saturatedPulse = pulse >= timer.Init.Period ? timer.Init.Period : pulse;
        __HAL_TIM_SET_COMPARE(&timer, channel, saturatedPulse);
    }
};