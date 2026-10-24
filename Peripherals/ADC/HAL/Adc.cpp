#include "Adc.hpp"

#if defined(HAL_ADC_MODULE_ENABLED)

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

        void Adc::Calibrate()
        {
            HAL_ADCEx_Calibration_Start(&adc, ADC_SINGLE_ENDED); //I dont need differental ended right now
        };

        void Adc::Start()
        {
            HAL_ADC_Start(&adc);
        };

        uint32_t Adc::Read() const
        {
            return HAL_ADC_GetValue(&adc);
        }

        float Adc::ReadVoltage() const
        {
            return Read() * voltageRef / maxValue;
        }
    };
};

#endif
