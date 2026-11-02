#pragma once
#include "GpioBase.hpp"

namespace Peripherals
{
    namespace RegisterLevel::STM32_L4
    {
        template<uint8_t pin_>
        class GpioAlternate : public GpioBase<GPIO_TypeDef, pin_>
        {
        protected:
            Gpio::AlternateFunction alternateFunction;

        public:
            GpioAlternate(const GpioAlternate& source) = default;
            GpioAlternate(GpioAlternate&& source) = default;
            GpioAlternate& operator=(const GpioAlternate& source) = delete;
            GpioAlternate& operator=(GpioAlternate&& source) = delete;
            GpioAlternate(GPIO_TypeDef* const port_
                        , Gpio::AlternateFunction alternateFunction_)
                : GpioBase<GPIO_TypeDef, pin_>(port_)
                , alternateFunction(alternateFunction_){}

            void Init(const Gpio::OTYPER otyperOption = GpioDefaults::otyperOption
                     , const Gpio::OSPEEDR ospeedrOption = GpioDefaults::ospeedrOption
                     , const Gpio::PUPDR pupdrOption = GpioDefaults::pupdrOption) override
            {
                this->EnableClock();
                this->template ConfigureMODER(Gpio::MODER::Alternate);
                this->template ConfigureOTYPER(otyperOption);
                this->template ConfigureOSPEEDR(ospeedrOption);
                this->template ConfigurePUPDR(pupdrOption);
                this->template ConfigureAlternateFunction(alternateFunction);
            }
        };
    }
}
