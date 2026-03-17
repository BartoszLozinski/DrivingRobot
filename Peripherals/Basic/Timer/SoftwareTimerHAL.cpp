#include "SoftwareTimerHAL.hpp"

#include "stm32l4xx_hal.h"

SoftwareTimerHAL::SoftwareTimerHAL(const uint32_t delay_)
    : TimerBase<SoftwareTimerHAL>(delay_)
{};

uint32_t SoftwareTimerHAL::Now_Impl() const
{
    return HAL_GetTick();
}
