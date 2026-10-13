#pragma once

#include <gtest/gtest.h>
#include "Peripherals/UART/Utils/RingBuffer.hpp"

template<typename T>
class RingBuffer_Fixture : public ::testing::Test
{
protected:
    T buffer;
};
