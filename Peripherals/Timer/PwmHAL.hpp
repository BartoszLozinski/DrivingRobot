#pragma once

#include "IPwm.hpp"

extern "C" 
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    class PwmHAL : public IPwm<PwmHAL>
    {
    friend IPwm<PwmHAL>;
    private:
        TIM_HandleTypeDef& timer;
        uint32_t channel = 0;

        void Start_Impl();
        void Stop_Impl();
        void SetPulse_Impl(const uint32_t pulse);
    public:
        PwmHAL() = delete;
        PwmHAL(const PwmHAL&) = delete;
        PwmHAL(PwmHAL&&) = delete;
        PwmHAL& operator=(const PwmHAL&) = delete;
        PwmHAL& operator=(PwmHAL&&) = delete;
        ~PwmHAL() = default;

        PwmHAL(TIM_HandleTypeDef& timer_, const uint32_t channel_);
    };
}