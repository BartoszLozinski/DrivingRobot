#pragma once
#include "../../Config.hpp"
#include "GpioEnums.hpp"

#include <cstdint>
#include <concepts>


namespace Peripherals
{
    namespace RegisterLevel
    {
        static constexpr std::array<uint32_t, 16> GPIO_ASCR_ASC = {
            GPIO_ASCR_ASC0, GPIO_ASCR_ASC1, GPIO_ASCR_ASC2, GPIO_ASCR_ASC3, GPIO_ASCR_ASC4, GPIO_ASCR_ASC5, GPIO_ASCR_ASC6, GPIO_ASCR_ASC7,
            GPIO_ASCR_ASC8, GPIO_ASCR_ASC9, GPIO_ASCR_ASC10, GPIO_ASCR_ASC11, GPIO_ASCR_ASC12, GPIO_ASCR_ASC13, GPIO_ASCR_ASC14, GPIO_ASCR_ASC15
        };

        template<typename T>
        concept GpioPort = requires(T port) {
            { port.MODER } -> std::convertible_to<volatile uint32_t&>;
            { port.OTYPER } -> std::convertible_to<volatile uint32_t&>;
            { port.OSPEEDR } -> std::convertible_to<volatile uint32_t&>;
            { port.PUPDR } -> std::convertible_to<volatile uint32_t&>;
            { port.IDR } -> std::convertible_to<volatile uint32_t&>;
            { port.ODR } -> std::convertible_to<volatile uint32_t&>;
            { port.BSRR } -> std::convertible_to<volatile uint32_t&>;
            { port.LCKR } -> std::convertible_to<volatile uint32_t&>;
            { port.AFR } -> std::convertible_to<volatile uint32_t(&)[2]>;
            { port.BRR } -> std::convertible_to<volatile uint32_t&>;
            { port.ASCR } -> std::convertible_to<volatile uint32_t&>;
        };

        template<GpioPort Port, uint8_t pin_>
        class GpioBase
        {
        protected:
            volatile Port* const port = nullptr;
            static constexpr uint8_t pin = pin_;

            void ConfigurePUPDR(const Gpio::PUPDR pupdrOption)
            {
                static constexpr uint8_t pupdrMask = 0b11;
                static constexpr uint8_t bitShift = 2 * pin;

                port->PUPDR &= ~(pupdrMask << bitShift);
                port->PUPDR |= (static_cast<uint32_t>(pupdrOption) << bitShift);
            }

            void ConfigureOTYPER(const Gpio::OTYPER otyperOption)
            {
                static constexpr uint8_t otyperMask = 0b1;
                port->OTYPER &= ~(otyperMask << pin);//clear bits
                port->OTYPER |= (static_cast<uint32_t>(otyperOption) << pin);
            }

            void ConfigureMODER(const Gpio::MODER moderOption)
            {
                static constexpr uint8_t moderMask = 0b11;
                static constexpr uint8_t bitShift = 2 * pin;

                port->MODER &= ~(moderMask << bitShift);//clear bits
                port->MODER |= (static_cast<uint32_t>(moderOption) << bitShift);
            }

            void ConfigureOSPEEDR(const Gpio::OSPEEDR ospeedrOption)
            {
                static constexpr uint8_t ospeedrMask = 0b11;
                static constexpr uint8_t bitShift = 2 * pin;

                port->OSPEEDR &= ~(ospeedrMask << bitShift);//clear bits
                port->OSPEEDR |= (static_cast<uint32_t>(ospeedrOption) << bitShift);
            }

            //Datasheet Pinouts and pin description
            //AF2 Datasheet Alternate functino 0 - 7 / 8 -15
            //RM 8.5.10 GPIO alternate function low/high register
            void ConfigureAlternateFunction(const Gpio::AlternateFunction af)
            {
                uint8_t LowOrHigh = pin <= 7 ? 0 : 1;
                static constexpr uint32_t AFMask = 0b1111;
                static constexpr uint32_t bitShift = 4 * (pin % 8); //AF is 4 bits wide
                port->AFR[LowOrHigh] &= ~(AFMask << bitShift);
                port->AFR[LowOrHigh] |= (static_cast<uint32_t>(af) << bitShift);
            }

            void EnableClock()
            {
                if (port == GPIOA)
                    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
                else if (port == GPIOB)
                    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
                else if (port == GPIOC)
                    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
                else if (port == GPIOD)
                    RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
            }

        public:
            GpioBase(Port* const port_)
                : port(port_)
            {
                static_assert(pin >= 0 && pin < 16, "Invalid pin number: needs to be in range of 0 - 15!");
            }

            virtual ~GpioBase() = default;
            virtual void Init(const Gpio::OTYPER otyperOption
                            , const Gpio::OSPEEDR ospeedrOption
                            , const Gpio::PUPDR pupdrOption) = 0;
        };

        struct GpioDefaults
        {
            static constexpr auto otyperOption = Gpio::OTYPER::PushPull;
            static constexpr auto ospeedrOption = Gpio::OSPEEDR::HighSpeed;
            static constexpr auto pupdrOption = Gpio::PUPDR::None;
        };
    }
}
