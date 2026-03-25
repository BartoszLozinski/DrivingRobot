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
        void Start() { static_cast<Implementation*>(this)->Start_Impl(); };
        void Stop() { static_cast<Implementation*>(this)->Stop_Impl(); };
        void SetPulse(const uint32_t pulse) { static_cast<Implementation*>(this)->SetPulse_Impl(pulse); };
        PwmState GetState() const { return static_cast<const Implementation*>(this)->GetState_Impl(); };
        void ResetCounter() { static_cast<Implementation*>(this)->ResetCounter_Impl(); }
        ~IPwm() = default;
    };
};