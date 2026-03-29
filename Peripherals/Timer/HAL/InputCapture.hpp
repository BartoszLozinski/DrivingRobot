#include "InputCaptureInterface.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class InputCapture : public InputCaptureInterface<InputCapture>
        {
        friend InputCaptureInterface<InputCapture>;
        private:
            TIM_HandleTypeDef& timer;
            uint32_t channel = 0;

            void Init();
            [[nodiscard]] uint32_t Read_Impl();
            
        public:
            InputCapture() = delete;
            InputCapture(const InputCapture&) = delete;
            InputCapture(InputCapture&&) = delete;
            InputCapture& operator=(const InputCapture&) = delete;
            InputCapture& operator=(InputCapture&&) = delete;

            explicit InputCapture(TIM_HandleTypeDef& timer_, const uint32_t channel_);
            uint32_t GetMaxCounter() const;
        };
    };
};