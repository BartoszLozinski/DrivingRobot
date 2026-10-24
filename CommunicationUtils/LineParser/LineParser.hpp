#pragma once

#include <concepts>
#include <optional>
#include <span>

template<typename T>
concept ByteReaderConcept = requires(T& byteReader)
{
    { byteReader.Read() } -> std::same_as<std::optional<uint8_t>>;
};

//TODO separate frmo uart - depend only on the ring buffer

namespace UcCommunication
{
    template<ByteReaderConcept ByteReaderT, std::size_t BufferSize = 64>
    class LineParser
    {
    private:
        ByteReaderT& byteReader;
        std::array<uint8_t, BufferSize> lineBuffer{};
        std::size_t lineIndex{ 0 };

        void ResetIndex()
        {
            lineIndex = 0;
        }

    public:
        explicit LineParser(ByteReaderT& byteReader_)
            : byteReader(byteReader_)
        {}

        std::optional<std::span<const uint8_t>> ReadLine()
        {
            while (auto byteOpt = byteReader.Read())
            {
                const uint8_t byte = static_cast<uint8_t>(*byteOpt);

                if (byte == '\n' || byte == '\r')
                {
                    if (lineIndex == 0)
                        continue; //ignore empty lines

                    const std::span<const uint8_t> line{ lineBuffer.data(), lineIndex };
                    ResetIndex();
                    return line;
                }
                
                if (lineIndex < lineBuffer.size())
                    lineBuffer[lineIndex++] = byte;
                else
                    ResetIndex(); //overflow discard
            }

            return std::nullopt;
        }
    };
}