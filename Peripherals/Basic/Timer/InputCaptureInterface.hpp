#pragma once

#include <cstdint>

namespace Peripherals
{
    template<typename Implementation>
    class InputCaptureInterface
    {
    protected:
        void Init() { static_cast<Implementation*>(this)->Init_Impl(); };

        InputCaptureInterface() { Init(); };
        ~InputCaptureInterface() = default; 
    public:
        [[nodiscard]] uint32_t Read() { return static_cast<Implementation*>(this)->Read_Impl(); };
    };
};
