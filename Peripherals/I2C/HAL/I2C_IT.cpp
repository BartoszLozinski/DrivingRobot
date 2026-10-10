#include "I2C_IT.hpp"

namespace Peripherals
{
    namespace HAL
    {
        I2C_IT::I2C_IT(I2C_HandleTypeDef& i2cHandle_) : i2cHandle(i2cHandle_) {}

        I2CResult I2C_IT::Write(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer)
        {
            if (state != I2CState::Idle)
                return I2CResult::Busy;

            const bool success = HAL_I2C_Mem_Write_IT( &i2cHandle
                                                    , deviceAddress << addressOffset
                                                    , memoryAddress
                                                    , I2C_MEMADD_SIZE_8BIT
                                                    , buffer.data()
                                                    , buffer.size()) == HAL_OK;

            state = success ? I2CState::TxBusy : I2CState::Error;
            return success ? I2CResult::Success : I2CResult::Error;
        }

        I2CResult I2C_IT::Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer)
        {
            if (state != I2CState::Idle)
                return I2CResult::Busy;

            const bool success = HAL_I2C_Mem_Read_IT(&i2cHandle
                                                    , deviceAddress << addressOffset
                                                    , memoryAddress
                                                    , I2C_MEMADD_SIZE_8BIT
                                                    , buffer.data()
                                                    , buffer.size()) == HAL_OK;

            state = success ? I2CState::RxBusy : I2CState::Error;
            return success ? I2CResult::Success : I2CResult::Error;
        }

        void I2C_IT::OnRxComplete()
        {
            if (state == I2CState::RxBusy)
                state = I2CState::RxDone;
        }

        void I2C_IT::OnTxComplete()
        {
            if (state == I2CState::TxBusy)
                state = I2CState::Idle;
        }

        void I2C_IT::NotifyDataIsRead()
        {
            if (state == I2CState::RxDone)
                state = I2CState::Idle;
        }

    }
}