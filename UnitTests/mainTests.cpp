#include <gtest/gtest.h>
#include "LineParser_Tests.hpp"
#include "RingBuffer_Tests.hpp"
#include "LPS25HB_Tests.hpp"
#include "ICM20948_Tests.hpp"
#include "WheelsController_Tests.hpp"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
