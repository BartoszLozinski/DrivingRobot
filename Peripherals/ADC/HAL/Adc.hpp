#pragma once

#include <cstdint>

#include "AdcBase.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class Adc : public AdcBase<Adc>
        {
        friend AdcBase<Adc>;
        private:
            ADC_HandleTypeDef& adc;
            void Start_Impl();
            void Calibrate_Impl();
            uint32_t Read_Impl();

        public:
            Adc() = delete;
            Adc(const Adc&) = delete;
            Adc(Adc&&) = delete;
            Adc& operator=(const Adc&) = delete;
            Adc& operator=(Adc&&) = delete;
            ~Adc() = default;

            Adc(ADC_HandleTypeDef& adc_);
        };
    };
}