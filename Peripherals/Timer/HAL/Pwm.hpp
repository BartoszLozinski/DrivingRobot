#pragma once

#include "TimerInterfaces/IPwm.hpp"

extern "C" 
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class Pwm : public IPwm
        {
        private:
            TIM_HandleTypeDef& timer;
            uint32_t channel = 0;
            
        public:
            Pwm() = delete;
            Pwm(const Pwm&) = delete;
            Pwm(Pwm&&) = delete;
            Pwm& operator=(const Pwm&) = delete;
            Pwm& operator=(Pwm&&) = delete;
            ~Pwm() = default;

            Pwm(TIM_HandleTypeDef& timer_, const uint32_t channel_);
            [[nodiscard]] uint16_t GetCounter() const override final;
            [[nodiscard]] uint16_t GetMaxCounter() const override final;
            [[nodiscard]] uint32_t GetPulse() const override final;
            [[nodiscard]] PwmState GetState() const override final;
            void ResetCounter() override final;
            void Start() override final;
            void Stop() override final;
            void SetPulse(const uint32_t pulse) override final;
        };
    };
}