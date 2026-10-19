#include <stdio.h>
#include <cstring>
#include <string_view>
#include <array>

#include "stm32CubeMXGenerated.h"

#include "Peripherals/Timer/RegisterLevel/SoftwareTimer.hpp"
#include "Peripherals/I2C/HAL/I2C_IT.hpp"
#include "Peripherals/UART/LineParser.hpp"
#include "Peripherals/Timer/HAL/Pwm.hpp"

#include "Devices/LPS25HB_Async.hpp"
#include "Devices/MCP23S08.hpp"
#include "Devices/ICM20948_Async.hpp"

#include "../Inc/peripheralsDefinition.h"

#include "Scheduler/Task.hpp"
#include "Scheduler/Scheduler.hpp"

#include "WheelsController/WheelsController.hpp"

Peripherals::HAL::I2C_IT i2c1IT{ hi2c1 };
Peripherals::HAL::I2C_IT i2c2IT{ hi2c2 };
Peripherals::HAL::Pwm tim3_ch1_pa6_left{ htim3, TIM_CHANNEL_1 }; //PA6
Peripherals::HAL::Pwm tim3_ch2_pa7_right{ htim3, TIM_CHANNEL_2 }; //PA7

Device::LPS25HB_Async lps25hbAsync{ i2c1IT };
Device::ICM20948_Async icm20948Async{ i2c2IT };

int main()
{
    /* MCU Configuration--------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    SystemClock_Config();
    /* Initialize all configured peripherals */

    MX_I2C1_Init();
    MX_I2C2_Init();
    MX_TIM3_Init();
    
    volatile float temperature = 0.0f;
    volatile uint32_t pressure = 0;
    WheelsController wheelsController{ tim3_ch1_pa6_left, tim3_ch2_pa7_right };

    Task<RegisterLevel::SoftwareTimer> readLPS25HBSensorTask{ 500, [&]()
    {
        if (lps25hbAsync.IsAwake())
        {
            if (const auto readTemp = lps25hbAsync.ReadTemperature(); readTemp)
                temperature = readTemp.value();

            if (const auto readPressure = lps25hbAsync.ReadPressure(); readPressure)
                pressure = readPressure.value();
        }
        else
        {
            lps25hbAsync.WakeUp();
        }
    }};

    Task<RegisterLevel::SoftwareTimer> printTemperaturePressureTask{ 2000, [&]()
    {
        char messageBuffer[48];
        snprintf(messageBuffer, sizeof(messageBuffer), "Temp: %.2f C, Pressure: %lu hPa\r\n", temperature, pressure);
        uart2.Transmit(reinterpret_cast<const uint8_t*>(messageBuffer), strlen(messageBuffer));
    }};

    Task<RegisterLevel::SoftwareTimer> ld2Task{ 150, [&]()
    {
        ld2.Toggle();
    }};

    auto throttleOption = Throttle::STOP;

    Task<RegisterLevel::SoftwareTimer> wheelsControllerTask{ 10, [&]()
    {
        // TODO:
        // add turning off option
        // and minimal pulse while turned on (simulation of not dropping too fast)
        // TODO add simple simulator for wheels control (something reading angular velocity of wheels (percentage))
        // and plotting wheels position of both
        // calculate arcs etc

        // communication app needs to be used, so keyboard press/release will be recognized
        if (const auto readValue = uart1.Read())
            throttleOption = wheelsController.ParseControlOption(readValue.value());

        wheelsController.Control(throttleOption);
    }};

    std::optional<AccelerometerGyroSample> accelerometerGyro = std::nullopt;

    bool initializationNotPrintedOrChanged = true;
    Task<RegisterLevel::SoftwareTimer> readICM20948Accelerometer{ 50, [&]()
    {
        if (icm20948Async.IsReady())
        {
            if (const auto readValue = icm20948Async.ReadAccelerometerGyro(); readValue)
                accelerometerGyro = readValue.value();
        }
        else
        {
            icm20948Async.Init(Device::ICM20948::AccelerometerSensitivity::g4, Device::ICM20948::GyroscopeSensitivity::dps500);
            if (initializationNotPrintedOrChanged)
            {
                initializationNotPrintedOrChanged = false;
                static constexpr char initMsg[] = "Initializing ICM-20948\r\n";
                uart2.Transmit(reinterpret_cast<const uint8_t*>(initMsg), strlen(initMsg));
            }
        }
    }};

    Task<RegisterLevel::SoftwareTimer> printAccelerometer{ 500, [&]()
    {
        if (accelerometerGyro)
        {
            const auto& value = accelerometerGyro.value();
            char messageBuffer[64];
            snprintf(messageBuffer, sizeof(messageBuffer), "Acc[%.2f, %.2f, %.2f] g\r\nGyro[%.2f, %.2f, %.2f] deg/s\r\n"
                    , value.accelX, value.accelY, value.accelZ
                    , value.gyroX, value.gyroY, value.gyroZ);

            uart2.Transmit(reinterpret_cast<const uint8_t*>(messageBuffer), strlen(messageBuffer));
        }
    }};
    
    Scheduler<Task<RegisterLevel::SoftwareTimer>, 6> scheduler{ { readLPS25HBSensorTask
                                                      , printTemperaturePressureTask
                                                      , ld2Task
                                                      , wheelsControllerTask
                                                      , readICM20948Accelerometer
                                                      , printAccelerometer } };

    // LPS25HB test
    uart1.ConfigureInterruptsPriority(IRQn_Type::USART1_IRQn, 1);
    uart1.Init(uart1Tx, uart1Rx, 9600);
    uart2.ConfigureInterruptsPriority(IRQn_Type::USART2_IRQn, 2);
    uart2.Init(uart2Tx, uart2Rx, 115200);
    ld2.Init();

    tim3_ch1_pa6_left.Start();
    tim3_ch2_pa7_right.Start();

    while (true)
    {
        // UART 1 Test - Bluetooth HC-06
        // connection at linux
        // sudo rfcomm connect 0 <Address>
        // on second terminal window: screen /dev/rfcomm0 9600

        uart1.ProcessTx();
        uart2.ProcessTx();
        
        scheduler.Run();
    }
}

void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

extern "C" void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        lps25hbAsync.OnRxComplete();
    }

    if (hi2c->Instance == I2C2)
    {
        icm20948Async.OnRxComplete();
    }
}

extern "C" void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        lps25hbAsync.OnTxComplete();
    }
    if (hi2c->Instance == I2C2)
    {
        icm20948Async.OnTxComplete();
    }
}

extern "C" void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        lps25hbAsync.OnTxComplete();
    }

    if (hi2c->Instance == I2C2)
    {
        icm20948Async.OnTxComplete();
    }
}

extern "C" void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        static constexpr uint8_t i2c1ErrorMsg[] = "I2C1 Error\r\n";
        uart2.Transmit(i2c1ErrorMsg, sizeof(i2c1ErrorMsg));
    }

    if (hi2c->Instance == I2C2)
    {
        static constexpr uint8_t i2c2ErrorMsg[] = "I2C2 Error\r\n";
        uart2.Transmit(i2c2ErrorMsg, sizeof(i2c2ErrorMsg));
    }
}

extern "C" void USART1_IRQHandler(void)
{
    uart1.IRQHandler();
}

extern "C" void USART2_IRQHandler(void)
{
    uart2.IRQHandler();
}
