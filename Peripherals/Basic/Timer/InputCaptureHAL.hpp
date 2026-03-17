#include "InputCaptureInterface.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    class InputCaptureHAL : public InputCaptureInterface<InputCaptureHAL>
    {
    friend InputCaptureInterface<InputCaptureHAL>;
    private:
        TIM_HandleTypeDef& timer;
        uint32_t channel = 0;

        void Init_Impl();
        uint32_t Read_Impl();
        
    public:
        InputCaptureHAL() = delete;
        InputCaptureHAL(const InputCaptureHAL&) = delete;
        InputCaptureHAL(InputCaptureHAL&&) = delete;
        InputCaptureHAL& operator=(const InputCaptureHAL&) = delete;
        InputCaptureHAL& operator=(InputCaptureHAL&&) = delete;

        explicit InputCaptureHAL(TIM_HandleTypeDef& timer_, const uint32_t channel_);
    };
};