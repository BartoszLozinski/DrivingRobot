#pragma once

#include "ICM20948Base.hpp"
#include "Peripherals/I2C/I2CBase.hpp"

struct AccelerometerReading
{
    float x;
    float y;
    float z;
};

namespace Device
{
    class ICM20948_Async : public ICM20948Base
    {
        using AccelerometerDirectionBuffer = std::array<uint8_t, 2>;
        using AccelerometerBuffer = std::array<uint8_t, 6>;

    private:
        enum class State
        {
            Idle,
            TransferScheduled,
            WakeUpScheduled,
            RegisterReadyToRead,
            RxAccelerometerReading,
            AccelerometerReadyToRead,
            WakeUpCompleted,
            Error,
        };

        Peripherals::I2CBase_IT& i2c;
        State state = State::Idle;
        bool isAwake = false;

        uint8_t currentPwrMgmt1Reg = 0x41; //default value after reset, device is in sleep mode (bit 7 = 1), auto selects best available clock source (bit 2:0 = 1-5)
        
        uint8_t readRegisterBuffer{};
        AccelerometerBuffer rawAccelerometerBuffer{};

        void StartRead(const uint16_t reg, std::span<uint8_t>(buffer), const State succesfulState);
        [[nodiscard]] std::optional<uint8_t> ReadRegister(uint8_t reg);

        // TODO add ReadAccelerometer X, Y, Z and Gyro
        /*
        start with X only for acc, when it works add rest of 2 DOF
        maybe create measurement struct for acc and for gyro
        add appropriate states and flags
        use buffer parsers, as in LPS25HB_Async
        */


    public:
        ICM20948_Async(Peripherals::I2CBase_IT& i2c_);

        [[nodiscard]] bool IsAwake() const;
        void WakeUp() override;
        void OnTxComplete();
        void OnRxComplete();
        std::optional<uint8_t> ReadWhoAmI();
        std::optional<AccelerometerReading> ReadAccelerometer();
    };
};