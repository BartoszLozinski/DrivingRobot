#pragma once
#include <cstdint>
#include <span>
#include <optional>

namespace Peripherals
{
    enum class I2CResult
    {
        Success,
        Busy,
        Error,        
    };

    enum class I2CState
    {
        Idle,
        RxBusy,
        TxBusy,
        RxDone,
        Error
    };

    class I2CInterface
    {
    public:
        virtual ~I2CInterface() = default;
        virtual I2CResult Write(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) = 0;
        [[nodiscard]] virtual I2CResult Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) = 0;
    };
}