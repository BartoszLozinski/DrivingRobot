#pragma once

#include <concepts>
#include <optional>
#include <span>

template<typename T>
concept UartConcept = requires(T uart)
{
    { uart.Read() } -> std::same_as<std::optional<uint8_t>>;
};

namespace UcCommunication
{
    template<UartConcept UartT, std::size_t BufferSize = 64>
    class LineParser
    {
    private:
        UartT& uart;
        std::array<uint8_t, BufferSize> lineBuffer{};
        std::size_t lineIndex{ 0 };
        volatile bool lastWasCarriageReturn{ false };

        void ResetIndex()
        {
            lineIndex = 0;
            lastWasCarriageReturn = false;
        }

    public:
        explicit LineParser(UartT& uart_)
            : uart(uart_)
        {}

        std::optional<std::span<const uint8_t>> ReadLine()
        {
            while (auto byteOpt = uart.Read())
            {
                const uint8_t byte = static_cast<uint8_t>(*byteOpt);

                if (byte == '\n')
                {
                    if (lastWasCarriageReturn)
                    {
                        lastWasCarriageReturn = false;
                        continue;
                    }

                    std::span<const uint8_t> line{ lineBuffer.data(), lineIndex };
                    ResetIndex();
                    return line;
                }
                
                if (byte == '\r')
                {
                    lastWasCarriageReturn = true;
                    std::span<const uint8_t> line{ lineBuffer.data(), lineIndex };
                    ResetIndex();
                    return line;
                }

                lastWasCarriageReturn = false;

                if (lineIndex < BufferSize)
                    lineBuffer[lineIndex++] = byte;
                else
                    ResetIndex();
            }

            return std::nullopt;
        }
    };
}