#include "InputCaptureHAL.hpp"

namespace Peripherals
{
    InputCaptureHAL::InputCaptureHAL(TIM_HandleTypeDef& timer_, const uint32_t channel_)
        : timer(timer_)
        , channel(channel_)
    {}

    void InputCaptureHAL::Init_Impl()
    {
        HAL_TIM_IC_Start(&timer, channel);
    };

    uint32_t InputCaptureHAL::Read_Impl()
    {
        return HAL_TIM_ReadCapturedValue(&timer, channel);
    }
};