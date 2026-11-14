#pragma once

#include "ISpi.hpp"

namespace Peripherals
{
    enum class SpiState
    {
        Idle,
        RxBusy,
        TxBusy,
        RxDone,
        Error
    };

    class ISpi_IT : public ISpi
    {
    public:
        virtual ~ISpi_IT() = default;
        virtual void OnRxComplete() = 0;
        virtual void OnTxComplete() = 0;
        virtual void NotifyDataIsRead() = 0;
    };
}
