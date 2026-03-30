#pragma once

#include "IGpioOutput.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class GpioOutput : public IGpioOutput<GpioOutput>
        {
        friend IGpioOutput<GpioOutput>;
        private:
            void Set_Impl();
            void Clear_Impl();
            void Toggle_Impl();
            GpioOutputState GetState_Impl() const;
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

        };
    };
};