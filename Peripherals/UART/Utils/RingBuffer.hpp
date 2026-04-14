#pragma once

#include <array>
#include <optional>

template<std::size_t BufferSize>
class RingBuffer
{
private:
    std::array<uint8_t, BufferSize> buffer{};
    // consider atomic head/tail
    volatile std::size_t head{ 0 };
    volatile std::size_t tail{ 0 };
    static constexpr std::size_t Next(std::size_t index) { return (index + 1 == BufferSize) ? 0 : index + 1; }
public:
    static_assert(BufferSize > 1, "Buffer size must be greater than 1.");
    [[nodiscard]] bool IsEmpty() const { return head == tail; }
    [[nodiscard]] bool IsFull() const { return Next(head) == tail; }
    [[nodiscard]] bool Push(const uint8_t& value)
    {
        const auto nextHead = Next(head);
        if (nextHead == tail) // Buffer is full
            return false;
        buffer[head] = value;
        head = nextHead;
        return true;
    }
    std::optional<uint8_t> Pop()
    {
        if (IsEmpty())
            return std::nullopt;
        const uint8_t value = buffer[tail];
        tail = Next(tail);
        return value;
    }
    [[nodiscard]] std::size_t Size() const
    {
        if (head >= tail)
            return head - tail;
        else
            return BufferSize - (tail - head);
    }
};