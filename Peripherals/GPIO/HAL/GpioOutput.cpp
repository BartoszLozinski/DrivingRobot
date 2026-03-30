#include "GpioOutput.hpp"

namespace Peripherals
{
    namespace HAL
    {
        GpioOutput::GpioOutput(GPIO_TypeDef& port_, const uint16_t pin_)
        : port(port_), pin(pin_)
        {}

        void GpioOutput::Set_Impl()
        {
            HAL_GPIO_WritePin(&port, pin, GPIO_PIN_SET);
        }

        void GpioOutput::Clear_Impl()
        {
            HAL_GPIO_WritePin(&port, pin, GPIO_PIN_RESET);
        }

        void GpioOutput::Toggle_Impl()
        {
            HAL_GPIO_TogglePin(&port, pin);
        }

        GpioOutputState GpioOutput::GetState_Impl() const
        {
            return (HAL_GPIO_ReadPin(&port, pin) == GPIO_PIN_SET) ? GpioOutputState::Set : GpioOutputState::Reset;
        }
    };
};