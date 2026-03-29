#include "InputCapture.hpp"

namespace Peripherals
{
    namespace HAL
    {
        InputCapture::InputCapture(TIM_HandleTypeDef& timer_, const uint32_t channel_)
            : timer(timer_)
            , channel(channel_)
        {
            Init();
        }

        void InputCapture::Init()
        {
            HAL_TIM_IC_Start(&timer, channel);
        };
 
        uint32_t InputCapture::GetMaxCounter() const
        {
            return __HAL_TIM_GET_AUTORELOAD(&timer);
        }

        uint32_t InputCapture::Read_Impl()
        {
            return HAL_TIM_ReadCapturedValue(&timer, channel);
        }
    };
};