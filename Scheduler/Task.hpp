#pragma once

#include <concepts>
#include <cstdint>
#include <functional>


template<typename T>
concept TimerConcept = requires(T t)
{
    t.Reset();
    { t.IsExpired() } -> std::convertible_to<bool>;
};

template<TimerConcept TimerType>
class Task
{
protected:
    TimerType timer;
    std::function<void()> function;
public:

    Task(const uint32_t periodMs, const std::function<void()>& function_)
        : timer(TimerType{ periodMs }), function(function_)
    {}

    void Update()
    {
        if (timer.IsExpired())
        {
            function();
            timer.Reset();
        }
    }
};
