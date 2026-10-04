#include "I2C.hpp"

namespace Peripherals
{
    namespace HAL
    {
        I2C::I2C(I2C_HandleTypeDef& i2cHandle_)
            : i2cHandle(i2cHandle_)
        {}


        I2CResult I2C::Write(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer)
        {
            static constexpr uint16_t memoryAddressSize = 1;

            return HAL_I2C_Mem_Write(&i2cHandle
                                    , deviceAddress << addressOffset
                                    , memoryAddress
                                    , memoryAddressSize
                                    , buffer.data()
                                    , buffer.size()
                                    , timeOut) == HAL_OK ? I2CResult::Success : I2CResult::Error;
        }

        I2CResult I2C::Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer)
        {
            if (buffer.empty())
                return I2CResult::Error;

            static constexpr uint16_t memoryAddressSize = 1;
            return HAL_I2C_Mem_Read( &i2cHandle
                                                , deviceAddress << addressOffset
                                                , memoryAddress
                                                , memoryAddressSize
                                                , buffer.data()
                                                , buffer.size()
                                                , timeOut) == HAL_OK ? I2CResult::Success : I2CResult::Error;
        }

        void I2C::SetTimeout(const uint32_t timeOut_)
        {
            timeOut = timeOut_;
        }
    }
}