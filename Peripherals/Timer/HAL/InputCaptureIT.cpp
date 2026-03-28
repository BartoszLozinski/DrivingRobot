#include "InputCaptureIT.hpp"

namespace Peripherals
{
    namespace HAL
    {
        InputCaptureIT::InputCaptureIT(TIM_HandleTypeDef& timer_, const uint32_t channel_)
            : timer(timer_)
            , channel(channel_)
        {}

        bool InputCaptureIT::DataUpdated() const
        {
            return dataUpdated;
        }

        uint32_t InputCaptureIT::GetValue()
        {
            //consider std::optional
            dataUpdated = false;
            return value;
        }

        //Needs to be called after MX_Init generated - HAL_TIM_Base_Init function
        void InputCaptureIT::Init()
        {
            HAL_TIM_Base_Start(&timer);
            HAL_TIM_IC_Start_IT(&timer, channel);
            __HAL_TIM_ENABLE_IT(&timer, TIM_IT_CC1 | TIM_IT_CC2);
            __HAL_TIM_ENABLE(&timer);
        };

        void InputCaptureIT::IrqHandler(const uint32_t activeChannel)
        {
            if (channel == activeChannel)
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