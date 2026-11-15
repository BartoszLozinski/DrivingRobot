#pragma once

#include "Peripherals/UART/RegisterLevel/Uart.hpp"
#include "Peripherals/GPIO/RegisterLevel/GpioAlternate.hpp"
#include "Peripherals/GPIO/RegisterLevel/GpioOutput.hpp"

Peripherals::RegisterLevel::STM32_L4::GpioAlternate<9> uart1Tx{ GPIOA, Peripherals::RegisterLevel::STM32_L4::Gpio::AlternateFunction::AF7 };
Peripherals::RegisterLevel::STM32_L4::GpioAlternate<10> uart1Rx{ GPIOA, Peripherals::RegisterLevel::STM32_L4::Gpio::AlternateFunction::AF7 };
Peripherals::RegisterLevel::STM32_L4::GpioAlternate<2> uart2Tx{ GPIOA, Peripherals::RegisterLevel::STM32_L4::Gpio::AlternateFunction::AF7 };
Peripherals::RegisterLevel::STM32_L4::GpioAlternate<3> uart2Rx{ GPIOA, Peripherals::RegisterLevel::STM32_L4::Gpio::AlternateFunction::AF7 };
Peripherals::RegisterLevel::STM32_L4::GpioOutput<4> spi1_cs{ GPIOC };
Peripherals::RegisterLevel::STM32_L476::Uart uart1{ USART1 };
Peripherals::RegisterLevel::STM32_L476::Uart<128> uart2{ USART2 };

Peripherals::RegisterLevel::STM32_L4::GpioOutput<0> tb6612fngStandby{ GPIOB };
Peripherals::RegisterLevel::STM32_L4::GpioOutput<0> tb6612fngAIn1{ GPIOC };
Peripherals::RegisterLevel::STM32_L4::GpioOutput<1> tb6612fngAIn2{ GPIOC };
Peripherals::RegisterLevel::STM32_L4::GpioOutput<2> tb6612fngBIn1{ GPIOC };
Peripherals::RegisterLevel::STM32_L4::GpioOutput<3> tb6612fngBIn2{ GPIOC };
