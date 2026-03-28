#include "InputCaptureIT.hpp"

namespace Peripherals
{
    namespace HAL
    {
        InputCaptureIT::InputCaptureIT(TIM_HandleTypeDef& timer_, const uint32_t channel_)
            : timer(timer_)
            , channel(channel_)
        {
            Init();
        }

        bool InputCaptureIT::DataUpdated() const
        {
            return dataUpdated;
        }

        uint32_t InputCaptureIT::GetValue()
        {
            dataUpdated = false;
            return value;
        }

        void InputCaptureIT::Init()
        {
            HAL_TIM_IC_Start_IT(&timer, channel);
        };

        void InputCaptureIT::IrqHandler()
        {
            if (timer.Channel == GetActiveChannel(channel))
            {
                value = Read();
                dataUpdated = true;
            }
        }

        uint32_t InputCaptureIT::Read_Impl()
        {
            return HAL_TIM_ReadCapturedValue(&timer, channel);
        }
    };
};