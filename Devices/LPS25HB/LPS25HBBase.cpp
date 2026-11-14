#include "LPS25HBBase.hpp"

namespace Device
{

    float LPS25HBBase::RecalculateRawTemperature(const int16_t rawTemp) const
    {
        // Formula provided by the datasheet
        return rawTemp / 480.0f + 42.5f; // *C
    }
    
    int32_t LPS25HBBase::RecalculateRawPressure(const int32_t rawPressure) const
    {
        // Formula provided by the datasheet
        return rawPressure / 4096; // hPa
    }
};
