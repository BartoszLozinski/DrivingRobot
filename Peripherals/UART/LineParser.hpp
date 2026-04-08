#pragma once

#include <concept>

template<typename T>
concept UartConcept = requires(T uart)
{
    { uart.Read() -> std::optional<uint8_t> };
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

        void ResetIndex()
        {
            lineIndex = 0;
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
                    std::span<const uint8_t> line{ lineBuffer.data(), lineIndex };
                    ResetIndex();
                    return line;
                }
                
                if (byte == '\r')
                    continue;
                
                if (lineIndex < BufferSize)
                    lineBuffer[lineIndex++] = byte;
                else
                    ResetIndex();
            }

            return std::nullopt; // No complete line available
        }
    };
}