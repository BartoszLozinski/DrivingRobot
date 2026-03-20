#include "Pwm.hpp"

namespace Peripherals
{
    namespace HAL
    {
        Pwm::Pwm(TIM_HandleTypeDef& timer_, const uint32_t channel_)
            : timer(timer_)
            , channel(channel_)
        {};

        void Pwm::Start_Impl()
        {
            HAL_TIM_PWM_Start(&timer, channel);
        }

        void Pwm::Stop_Impl()
        {
            HAL_TIM_PWM_Stop(&timer, channel);
        };

        void Pwm::SetPulse_Impl(const uint32_t pulse)
        {
            const uint32_t saturatedPulse = pulse >= timer.Init.Period ? timer.Init.Period : pulse;
            __HAL_TIM_SET_COMPARE(&timer, channel, saturatedPulse);
        }
    }
};