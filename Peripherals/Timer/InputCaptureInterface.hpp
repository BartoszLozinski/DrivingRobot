#pragma once

#include <cstdint>

namespace Peripherals
{
    template<typename Implementation>
    class InputCaptureInterface
    {
    protected:
        InputCaptureInterface() {};
        ~InputCaptureInterface() = default; 
    public:
        [[nodiscard]] uint32_t Read() { return static_cast<Implementation*>(this)->Read_Impl(); };
    };
};
