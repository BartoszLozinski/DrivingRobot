#pragma once
#include "I2CInterface.hpp"

namespace Peripherals
{
    class I2CInterface_IT : public I2CInterface
    {
    public:
        virtual ~I2CInterface_IT() = default;
        virtual void OnRxComplete() = 0;
        virtual void OnTxComplete() = 0;
        virtual void NotifyDataIsRead() = 0;
        [[nodiscard]] virtual Peripherals::I2CState GetState() const = 0;
    };
}