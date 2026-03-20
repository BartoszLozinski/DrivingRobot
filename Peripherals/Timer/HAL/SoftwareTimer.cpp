#include "SoftwareTimer.hpp"

#include "stm32l4xx_hal.h"

namespace HAL
{
    SoftwareTimer::SoftwareTimer(const uint32_t delay_)
        : TimerBase<SoftwareTimer>(delay_)
    {};

    uint32_t SoftwareTimer::Now_Impl() const
    {
        return HAL_GetTick();
    }
};
