#pragma once

#include "Interfaces/IGpioInput.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class GpioInput : public IGpioInput
        {
        private:
            GPIO_TypeDef& port;
            uint16_t pin;
        public:
            GpioInput() = delete;
            GpioInput(const GpioInput&) = delete;
            GpioInput(GpioInput&&) = delete;
            GpioInput& operator=(const GpioInput&) = delete;
            GpioInput& operator=(GpioInput&&) = delete;
            ~GpioInput() = default;

            GpioInput(GPIO_TypeDef& port_, const uint16_t pin_);
            uint32_t Read() const override;
        };
    };
};
