#pragma once

#include "Interfaces/IGpioOutput.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class GpioOutput : public IGpioOutput
        {
        private:
            GPIO_TypeDef& port;
            uint16_t pin;

        public:
            GpioOutput() = delete;
            GpioOutput(const GpioOutput&) = delete;
            GpioOutput(GpioOutput&&) = delete;
            GpioOutput& operator=(const GpioOutput&) = delete;
            GpioOutput& operator=(GpioOutput&&) = delete;
            ~GpioOutput() = default;

            GpioOutput(GPIO_TypeDef& port_, const uint16_t pin_);

            void Set() override final;
            void Clear() override final;
            void Toggle() override final;
            GpioState GetState() const override final;
        };
    };
};