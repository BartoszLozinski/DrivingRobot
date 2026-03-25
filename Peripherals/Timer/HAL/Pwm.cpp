#include "Pwm.hpp"
#include "../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include/stm32l4xx.h"

namespace Peripherals
{
    namespace HAL
    {
        Pwm::Pwm(TIM_HandleTypeDef& timer_, const uint32_t channel_)
            : timer(timer_)
            , channel(channel_)
        {
            HAL_TIM_Base_Init(&timer);
            //HAL_TIM_PWM_Init(&timer);
            HAL_TIM_OnePulse_Init(&timer, channel);
        };

        PwmState Pwm::GetState_Impl() const
        {
            return static_cast<PwmState>(HAL_TIM_PWM_GetState(&timer));
        };

        void Pwm::Start_Impl()
        {
            HAL_TIM_Base_Start(&timer);
            //HAL_TIM_PWM_Start(&timer, channel);
            HAL_TIM_OnePulse_Start(&timer, channel);
            //timer.Instance->CR1 |= TIM_CR1_CEN;
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

        void Pwm::ResetCounter_Impl()
        {
            __HAL_TIM_SET_COUNTER(&timer, 0);
        }
    }
};