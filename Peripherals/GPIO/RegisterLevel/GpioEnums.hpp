#pragma once

namespace Peripherals
{
    namespace RegisterLevel::STM32_L4
    {
        namespace Gpio
        {
            enum class PUPDR
            {
                // GPIOx_PUPDR (GPIO port pull-up/pull-down register):
                // Reset value: 0x6400 0000 (port A) - pin15 [pull-up], pin14 [pull-down], others [No pull-up/pull-down]
                // Reset value: 0x0000 0100 (port B) - pin4 [pull-up]
                // Reset value: 0x0000 0000 (for other ports)
                None = 0b00, // 00 = No pull-up, no pull-down
                PullUp = 0b01, // 01 = Pull-up
                PullDown = 0b10,// 10 = Pull-down
                /*Reserved,// 11 = Reserved - but you shouldn't use reserved pin*/
            };

            enum class OTYPER
            {
                // GPIOx_OTYPER (GPIO port output type register):
                //reset value 0x0000 0000
                PushPull = 0b0, // 0 = Output push-pull (reset state)
                OpenDrain = 0b1,// 1 = Output open-drain
            };

            enum class MODER
            {
                // STM32L476RGT6 Reference Manual:
                // GPIOx_MODER (GPIO port mode register) controls the mode of each pin
                //Reset value: 0xABFF FFFF (port A) - 1010 1011 1111 1111 1111 1111 1111 1111
                //Reset value: 0xFFFF FEBF (port B) - 1111 1111 1111 1111 1111 1110 1011 1111
                Input = 0b00, // 00 = Input mode
                Output = 0b01, // 01 = General purpose output mode
                Alternate = 0b10, // 10 = Alternate function mode
                Analog = 0b11, // 11 = Analog mode (reset state for most cases)
            };

            enum class OSPEEDR
            {
                // STM32L476RGT6 RM: 8.5.3 GPIO port output speed register
                // Reset value: 0x0C00 0000 for port A
                // Reset value: 0x0000 0000 for other ports
                LowSpeed = 0b00, // 00
                MediumSpeed = 0b01, // 01
                HighSpeed = 0b10, // 10
                VeryHighSpeed = 0b11, //11
                //Refer to the device datasheet for the frequency specifications
                //and the power supply and load conditions for each speed...
            };

            //Datasheet Pinouts and pin description
            //AF2 Datasheet Alternate functinon 0 - 15
            enum class AlternateFunction
            {
                AF0 = 0b0000,
                AF1 = 0b0001,
                AF2 = 0b0010,
                AF3 = 0b0011,
                AF4 = 0b0100,
                AF5 = 0b0101,
                AF6 = 0b0110,
                AF7 = 0b0111,
                AF8 = 0b1000,
                AF9 = 0b1001,
                AF10 = 0b1010,
                AF11 = 0b1011,
                AF12 = 0b1100,
                AF13 = 0b1101,
                AF14 = 0b1110,
                AF15 = 0b1111
            };

            enum class Trigger
            {
                Rising,
                Falling,
                Both
            };
        }
    }
}