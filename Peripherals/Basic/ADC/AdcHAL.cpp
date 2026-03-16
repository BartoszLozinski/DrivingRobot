#include "AdcHAL.hpp"

namespace Peripherals
{
    AdcHAL::AdcHAL(ADC_HandleTypeDef& adc_)
        : adc(adc_)
    {
        Calibrate();
        Start();
    };

    void AdcHAL::Calibrate_Impl()
    {
        HAL_ADCEx_Calibration_Start(&adc, ADC_SINGLE_ENDED); //I dont need differental ended right now
    };

    void AdcHAL::Start_Impl()
    {
        HAL_ADC_Start(&adc);
    };

    uint32_t AdcHAL::Read_Impl()
    {
        return HAL_ADC_GetValue(&adc);
    }
};