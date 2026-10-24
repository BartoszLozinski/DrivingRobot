#pragma once
#include "GpioBase.hpp"
#include "Interfaces/IGpioOutput.hpp"

namespace Peripherals
{
    namespace RegisterLevel
    {
        template<uint8_t pin_>
        class GpioOutput : public GpioBase<GPIO_TypeDef, pin_>
                         , public IGpioOutput
        {
            using GpioBase<GPIO_TypeDef, pin_>::port;
            using GpioBase<GPIO_TypeDef, pin_>::pin;

        public:
            GpioOutput(const GpioOutput& source) = delete;
            GpioOutput(GpioOutput&& source) = delete;
            GpioOutput& operator=(const GpioOutput& source) = delete;
            GpioOutput& operator=(GpioOutput&& source) = delete;
            GpioOutput(GPIO_TypeDef* const port_)
                : GpioBase<GPIO_TypeDef, pin_>(port_)
            {}

            void Init(const Gpio::OTYPER otyperOption = GpioDefaults::otyperOption
                     , const Gpio::OSPEEDR ospeedrOption = GpioDefaults::ospeedrOption
                     , const Gpio::PUPDR pupdrOption = GpioDefaults::pupdrOption) override
            {
                this->EnableClock();
                this->template ConfigureMODER(Gpio::MODER::Output);
                this->template ConfigureOTYPER(otyperOption);
                this->template ConfigureOSPEEDR(ospeedrOption);
                this->template ConfigurePUPDR(pupdrOption);
            }

            ~GpioOutput() = default;

            //Output data register ODR - reference manual 8.5.6
            GpioState GetState() const override final { return port->ODR & PinMask<pin>() ? GpioState::High : GpioState::Low; }
            void Set() override final { port->BSRR |= (0b1 << pin); }
            void Clear() override final
            {
                static constexpr uint8_t bitShiftOffset = 16;
                static constexpr uint8_t bitShift = bitShiftOffset + pin; //bit resets are 16-32
                port->BSRR |= (0b1 << bitShift);
            }
            void Toggle() override final { port->ODR ^= (0b1 << pin); /*Bitwise XOR*/} //ODR has 16 bits

        };
    }
}
