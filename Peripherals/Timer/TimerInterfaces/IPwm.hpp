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

    class IPwm
    {
    public:
        IPwm() = default;
        virtual ~IPwm() = default;

        [[nodiscard]] virtual uint16_t GetCounter() const = 0;
        [[nodiscard]] virtual uint16_t GetMaxCounter() const = 0;
        [[nodiscard]] virtual uint32_t GetPulse() const = 0;
        [[nodiscard]] virtual PwmState GetState() const = 0;
        virtual void ResetCounter() = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void SetPulse(uint32_t pulse) = 0;
        virtual void SetPulsePercent(const uint8_t pulsePercent) = 0;

    };
};