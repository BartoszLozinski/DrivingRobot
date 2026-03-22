#include "Adc.hpp"

namespace Peripherals
{
    namespace HAL
    {
        Adc::Adc(ADC_HandleTypeDef& adc_)
            : adc(adc_)
        {
            Calibrate();
            Start();
        };

        void Adc::Calibrate_Impl()
        {
            HAL_ADCEx_Calibration_Start(&adc, ADC_SINGLE_ENDED); //I dont need differental ended right now
        };

        void Adc::Start_Impl()
        {
            HAL_ADC_Start(&adc);
        };

        uint32_t Adc::Read_Impl() const
        {
            return HAL_ADC_GetValue(&adc);
        }
    };
};