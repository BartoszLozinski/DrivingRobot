#pragma once
#include "GpioBase.hpp"

namespace Peripherals
{
    namespace RegisterLevel::STM32_L4
    {
        template<uint8_t pin_>
        class GpioAnalog : public GpioBase<GPIO_TypeDef, pin_>
        {
            using GpioBase<GPIO_TypeDef, pin_>::port;
            using GpioBase<GPIO_TypeDef, pin_>::pin;

        protected:

            void ConnectAnalogSwitch(const uint8_t pin)
            {
                port->ASCR |= GPIO_ASCR_ASC[pin];
            }

        public:
            GpioAnalog(const GpioAnalog& source) = default;
            GpioAnalog(GpioAnalog&& source) = default;
            GpioAnalog& operator=(const GpioAnalog& source) = delete;
            GpioAnalog& operator=(GpioAnalog&& source) = delete;
            GpioAnalog(GPIO_TypeDef* const port_)
                : GpioBase<GPIO_TypeDef, pin_>(port_)
            {
                ConfigureAsAnalog();
            }

            void Init(const Gpio::OTYPER otyperOption = GpioDefaults::otyperOption
                     , const Gpio::OSPEEDR ospeedrOption = Gpio::OSPEEDR::LowSpeed;
                     , const Gpio::PUPDR pupdrOption = GpioDefaults::pupdrOption) override
            {
                this->EnableClock();
                this->template ConfigureMODER(Gpio::MODER::Analog);
                this->template ConfigureOTYPER(otyperOption);
                this->template ConfigureOSPEEDR(ospeedrOption);
                this->template ConfigurePUPDR(pupdrOption);
                ConnectAnalogSwitch(pin);
            }
        };
    }
}
