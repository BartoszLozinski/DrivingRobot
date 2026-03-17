#pragma once

#include <cstdint>

#include "AdcBase.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    class AdcHAL : public AdcBase<AdcHAL>
    {
    friend AdcBase<AdcHAL>;
    private:
        ADC_HandleTypeDef& adc;
        void Start_Impl();
        void Calibrate_Impl();
        uint32_t Read_Impl();

    public:
        AdcHAL() = delete;
        AdcHAL(const AdcHAL&) = delete;
        AdcHAL(AdcHAL&&) = delete;
        AdcHAL& operator=(const AdcHAL&) = delete;
        AdcHAL& operator=(AdcHAL&&) = delete;
        ~AdcHAL() = default;

        AdcHAL(ADC_HandleTypeDef& adc_);
    };
}