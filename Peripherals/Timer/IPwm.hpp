#pragma once

#include <cstdint>

namespace Peripherals
{
    enum class PwmState
    {
        RESET             = 0x00U,
        READY             = 0x01U,
        BUSY              = 0x02U,
        TIMEOUT           = 0x03U,
        ERROR             = 0x04U,
    };

    template<typename Implementation>
    class IPwm
    {
    protected:
        IPwm() = default;
    public:
        [[nodiscard]] uint16_t GetCounter() const { return static_cast<const Implementation*>(this)->GetCounter_Impl(); };
        [[nodiscard]] uint16_t GetMaxCounter() const { return static_cast<const Implementation*>(this)->GetMaxCounter_Impl(); };
        [[nodiscard]] PwmState GetState() const { return static_cast<const Implementation*>(this)->GetState_Impl(); };
        void ResetCounter() { static_cast<Implementation*>(this)->ResetCounter_Impl(); }
        void Start() { static_cast<Implementation*>(this)->Start_Impl(); };
        void Stop() { static_cast<Implementation*>(this)->Stop_Impl(); };
        void SetPulse(uint32_t pulse) { static_cast<Implementation*>(this)->SetPulse_Impl(pulse); };
        
        ~IPwm() = default;
    };
};