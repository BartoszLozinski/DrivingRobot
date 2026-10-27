#include "SoftwareTimer.hpp"

#include "../../Core/Inc/stm32l4xx_it.h"

namespace RegisterLevel::STM32_L4
{
    SoftwareTimer::SoftwareTimer(const uint32_t delay_)
        : TimerBase<SoftwareTimer>(delay_)
    {};

    uint32_t SoftwareTimer::Now_Impl() const
    {
        return systemTickCounter;
    }
};