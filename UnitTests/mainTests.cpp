#include <gtest/gtest.h>
#include "HC_SR04_Tests.hpp"
#include "LM35_Tests.hpp"
#include "LineParser_Tests.hpp"
#include "RingBuffer_Tests.hpp"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
