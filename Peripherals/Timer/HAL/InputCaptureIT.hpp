#pragma once

#include "InputCaptureInterface.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class InputCaptureIT : public InputCaptureInterface<InputCaptureIT>
        {
        friend InputCaptureInterface<InputCaptureIT>;
        private:
            TIM_HandleTypeDef& timer;
            uint32_t channel = 0;
            volatile bool dataUpdated = false;
            volatile uint32_t value = 0;

            void Init();
            [[nodiscard]] uint32_t Read_Impl();

            static constexpr uint32_t GetActiveChannel(const uint32_t ch)
            {
                switch (ch)
                {
                    case TIM_CHANNEL_1: return HAL_TIM_ACTIVE_CHANNEL_1;
                    case TIM_CHANNEL_2: return HAL_TIM_ACTIVE_CHANNEL_2;
                    case TIM_CHANNEL_3: return HAL_TIM_ACTIVE_CHANNEL_3;
                    case TIM_CHANNEL_4: return HAL_TIM_ACTIVE_CHANNEL_4;
                    case TIM_CHANNEL_5: return HAL_TIM_ACTIVE_CHANNEL_5;
                    case TIM_CHANNEL_6: return HAL_TIM_ACTIVE_CHANNEL_6;
                    default: return 0;
                };
            }
            
        public:
            InputCaptureIT() = delete;
            InputCaptureIT(const InputCaptureIT&) = delete;
            InputCaptureIT(InputCaptureIT&&) = delete;
            InputCaptureIT& operator=(const InputCaptureIT&) = delete;
            InputCaptureIT& operator=(InputCaptureIT&&) = delete;

            explicit InputCaptureIT(TIM_HandleTypeDef& timer_, const uint32_t channel_);

            [[nodiscard]] bool DataUpdated() const;
            [[nodiscard]] uint32_t GetValue();
            void IrqHandler();
        };
    };
};