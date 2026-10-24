#pragma once

#if defined(HAL_ADC_MODULE_ENABLED)

#include <cstdint>
#include "AdcInterfaces/IAdc.hpp"

extern "C"
{
    #include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class Adc : public IAdc
        {
        private:
            ADC_HandleTypeDef& adc;
            static constexpr uint16_t maxValue = 4096;
            static constexpr float voltageRef = 3.3f;

        public:            
            Adc() = delete;
            Adc(const Adc&) = delete;
            Adc(Adc&&) = delete;
            Adc& operator=(const Adc&) = delete;
            Adc& operator=(Adc&&) = delete;
            ~Adc() = default;

            explicit Adc(ADC_HandleTypeDef& adc_);
            void Start() override final;
            void Calibrate() override final;
            uint32_t Read() const override final;
        };
    };
}

#endif
