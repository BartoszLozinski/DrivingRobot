#pragma once

#include "IPwm.hpp"

extern "C" 
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class Pwm : public IPwm<Pwm>
        {
        friend IPwm<Pwm>;
        private:
            TIM_HandleTypeDef& timer;
            uint32_t channel = 0;

            void Start_Impl();
            void Stop_Impl();
            void SetPulse_Impl(const uint32_t pulse);
            PwmState GetState_Impl() const;
            void ResetCounter_Impl();

            [[nodiscard]] bool IsOnePulseEnabled() const;
            
        public:
            Pwm() = delete;
            Pwm(const Pwm&) = delete;
            Pwm(Pwm&&) = delete;
            Pwm& operator=(const Pwm&) = delete;
            Pwm& operator=(Pwm&&) = delete;
            ~Pwm() = default;

            Pwm(TIM_HandleTypeDef& timer_, const uint32_t channel_);
        };
    };
}