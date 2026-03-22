#pragma once

#include <concepts>
#include <cstdint>

template<typename T>
concept AdcConcept= requires (const T& adc)
{
    { adc.ReadVoltage() } -> std::same_as<float>; 
};

namespace Device
{
    //Temperature sensor
    template<AdcConcept Adc_T>
    class LM35
    {
    private:
        Adc_T& adc;

    public:
        LM35(Adc_T& adc_)
            : adc(adc_)
        {};

        float ReadTempC() const
        {
            static constexpr uint8_t V_TO_Celcius = 100;
            return adc.ReadVoltage() * V_TO_Celcius;
        }
    };
};