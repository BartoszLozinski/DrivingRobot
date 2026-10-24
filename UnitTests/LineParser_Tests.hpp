#pragma once

#include <gtest/gtest.h>
#include <queue>
#include "CommunicationUtils/LineParser/LineParser.hpp"

struct FakeUart
{
    std::queue<uint8_t> dataQueue;

    void AddByte(uint8_t byte);
    void AddData(const std::string& str);    
    void Clear();
    std::optional<uint8_t> Read();
};

class LineParser_Fixture : public ::testing::Test
{
protected:
    FakeUart uart;
    UcCommunication::LineParser<FakeUart> parser{ uart };
};

