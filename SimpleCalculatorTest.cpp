#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "SimpleCalculator.h"
#include "InMemoryHistory.h"

using ::testing::_;
using ::testing::Return;
using ::testing::InSequence;

namespace calc
{

// Mock for IHistory
class MockHistory : public IHistory
{
public:
    MOCK_METHOD(void, AddEntry, (const std::string& operation), (override));
    MOCK_METHOD(std::vector<std::string>, GetLastOperations, (size_t count), (const, override));
};

// Mock for ICalculator
class MockCalculator : public ICalculator
{
public:
    MOCK_METHOD(void, SetHistory, (IHistory& history), (override));
    MOCK_METHOD(int, Add, (int a, int b), (override));
    MOCK_METHOD(int, Subtract, (int a, int b), (override));
    MOCK_METHOD(int, Multiply, (int a, int b), (override));
    MOCK_METHOD(int, Divide, (int a, int b), (override));
};

class SimpleCalculatorTest : public ::testing::Test
{
protected:
    MockHistory mockHistory;
    SimpleCalculator calculator{mockHistory};
};

TEST_F(SimpleCalculatorTest, Add_CorrectResult)
{
    EXPECT_CALL(mockHistory, AddEntry("2 + 2 = 4"));
    EXPECT_EQ(calculator.Add(2, 2), 4);
}

TEST_F(SimpleCalculatorTest, Subtract_CorrectResult)
{
    EXPECT_CALL(mockHistory, AddEntry("5 - 3 = 2"));
    EXPECT_EQ(calculator.Subtract(5, 3), 2);
}

TEST_F(SimpleCalculatorTest, Multiply_CorrectResult)
{
    EXPECT_CALL(mockHistory, AddEntry("4 * 3 = 12"));
    EXPECT_EQ(calculator.Multiply(4, 3), 12);
}

TEST_F(SimpleCalculatorTest, Divide_CorrectResult)
{
    EXPECT_CALL(mockHistory, AddEntry("10 / 2 = 5"));
    EXPECT_EQ(calculator.Divide(10, 2), 5);
}

TEST_F(SimpleCalculatorTest, Divide_ByZero_Throws)
{
    EXPECT_THROW(calculator.Divide(10, 0), std::exception);
}

TEST_F(SimpleCalculatorTest, Add_BoundaryValues)
{
    EXPECT_CALL(mockHistory, AddEntry("2147483647 + 0 = 2147483647"));
    EXPECT_EQ(calculator.Add(std::numeric_limits<int>::max(), 0), std::numeric_limits<int>::max());

    EXPECT_CALL(mockHistory, AddEntry("-2147483648 + 0 = -2147483648"));
    EXPECT_EQ(calculator.Add(std::numeric_limits<int>::min(), 0), std::numeric_limits<int>::min());
}

TEST_F(SimpleCalculatorTest, Multiply_BoundaryValues)
{
    EXPECT_CALL(mockHistory, AddEntry("2147483647 * 1 = 2147483647"));
    EXPECT_EQ(calculator.Multiply(std::numeric_limits<int>::max(), 1), std::numeric_limits<int>::max());

    EXPECT_CALL(mockHistory, AddEntry("-2147483648 * 1 = -2147483648"));
    EXPECT_EQ(calculator.Multiply(std::numeric_limits<int>::min(), 1), std::numeric_limits<int>::min());
}

TEST_F(SimpleCalculatorTest, MultipleOperations_LogsCorrectly)
{
    InSequence seq;
    EXPECT_CALL(mockHistory, AddEntry("2 + 3 = 5"));
    EXPECT_CALL(mockHistory, AddEntry("10 - 4 = 6"));
    EXPECT_CALL(mockHistory, AddEntry("5 * 2 = 10"));

    calculator.Add(2, 3);
    calculator.Subtract(10, 4);
    calculator.Multiply(5, 2);
}

TEST_F(SimpleCalculatorTest, SetHistory_LogsToNewHistory)
{
    MockHistory newHistory;
    calculator.SetHistory(newHistory);

    EXPECT_CALL(newHistory, AddEntry("2 + 2 = 4"));
    calculator.Add(2, 2);
}

TEST(InMemoryHistoryTest, AddEntry_StoresCorrectly)
{
    InMemoryHistory history;
    history.AddEntry("2 + 2 = 4");
    history.AddEntry("5 - 3 = 2");

    auto operations = history.GetLastOperations(2);
    ASSERT_EQ(operations.size(), 2);
    EXPECT_EQ(operations[0], "2 + 2 = 4");
    EXPECT_EQ(operations[1], "5 - 3 = 2");
}

TEST(InMemoryHistoryTest, GetLastOperations_RespectsCount)
{
    InMemoryHistory history;
    history.AddEntry("1 + 1 = 2");
    history.AddEntry("2 + 2 = 4");
    history.AddEntry("3 + 3 = 6");

    auto operations = history.GetLastOperations(2);
    ASSERT_EQ(operations.size(), 2);
    EXPECT_EQ(operations[0], "2 + 2 = 4");
    EXPECT_EQ(operations[1], "3 + 3 = 6");
}

TEST(InMemoryHistoryTest, GetLastOperations_MoreThanAvailable)
{
    InMemoryHistory history;
    history.AddEntry("1 + 1 = 2");

    auto operations = history.GetLastOperations(5);
    ASSERT_EQ(operations.size(), 1);
    EXPECT_EQ(operations[0], "1 + 1 = 2");
}

TEST_F(SimpleCalculatorTest, Divide_FractionalResult_Truncates)
{
    EXPECT_CALL(mockHistory, AddEntry("7 / 2 = 3"));
    EXPECT_EQ(calculator.Divide(7, 2), 3);
}

TEST(InMemoryHistoryTest, History_NoLimitDefined)
{
    InMemoryHistory history;
    for (size_t i = 0; i < 10000; ++i)
    {
        history.AddEntry("1 + 1 = 2");
    }

    auto operations = history.GetLastOperations(10000);
    EXPECT_EQ(operations.size(), 10000); 
}

TEST(MockCalculatorTest, CalculatorMock_InteractsWithHistory)
{
    InMemoryHistory history;
    MockCalculator mockCalculator;

    EXPECT_CALL(mockCalculator, Add(2, 2)).WillOnce(Return(4));
    EXPECT_CALL(mockCalculator, SetHistory(_));

    mockCalculator.SetHistory(history);
    int result = mockCalculator.Add(2, 2);
    EXPECT_EQ(result, 4);

    history.AddEntry("2 + 2 = 4");
    auto operations = history.GetLastOperations(1);
    ASSERT_EQ(operations.size(), 1);
    EXPECT_EQ(operations[0], "2 + 2 = 4");
}

TEST_F(SimpleCalculatorTest, Add_NegativeNumbers)
{
    EXPECT_CALL(mockHistory, AddEntry("-2 + -3 = -5"));
    EXPECT_EQ(calculator.Add(-2, -3), -5);
}

TEST_F(SimpleCalculatorTest, Subtract_NegativeNumbers)
{
    EXPECT_CALL(mockHistory, AddEntry("-5 - -3 = -2"));
    EXPECT_EQ(calculator.Subtract(-5, -3), -2);
}

TEST_F(SimpleCalculatorTest, Multiply_NegativeNumbers)
{
    EXPECT_CALL(mockHistory, AddEntry("-4 * -3 = 12"));
    EXPECT_EQ(calculator.Multiply(-4, -3), 12);
}

TEST_F(SimpleCalculatorTest, Divide_NegativeNumbers)
{
    EXPECT_CALL(mockHistory, AddEntry("-10 / -2 = 5"));
    EXPECT_EQ(calculator.Divide(-10, -2), 5);
}

TEST_F(SimpleCalculatorTest, Add_Overflow_Throws)
{
    EXPECT_THROW(calculator.Add(std::numeric_limits<int>::max(), 1), std::overflow_error);
}

TEST_F(SimpleCalculatorTest, Add_Zero)
{
    EXPECT_CALL(mockHistory, AddEntry("5 + 0 = 5"));
    EXPECT_EQ(calculator.Add(5, 0), 5);
}

TEST_F(SimpleCalculatorTest, Multiply_Zero)
{
    EXPECT_CALL(mockHistory, AddEntry("5 * 0 = 0"));
    EXPECT_EQ(calculator.Multiply(5, 0), 0);
}

TEST(InMemoryHistoryTest, GetLastOperations_EmptyHistory)
{
    InMemoryHistory history;
    auto operations = history.GetLastOperations(1);
    EXPECT_TRUE(operations.empty());
}

TEST(InMemoryHistoryTest, GetLastOperations_ZeroCount)
{
    InMemoryHistory history;
    history.AddEntry("1 + 1 = 2");
    auto operations = history.GetLastOperations(0);
    EXPECT_TRUE(operations.empty());
}

TEST_F(SimpleCalculatorTest, Add_MixedNumbers)
{
    EXPECT_CALL(mockHistory, AddEntry("-2 + 5 = 3"));
    EXPECT_EQ(calculator.Add(-2, 5), 3);
}

TEST_F(SimpleCalculatorTest, Subtract_MixedNumbers)
{
    EXPECT_CALL(mockHistory, AddEntry("5 - -3 = 8"));
    EXPECT_EQ(calculator.Subtract(5, -3), 8);
}

TEST_F(SimpleCalculatorTest, Subtract_Overflow_Throws)
{
    int min = std::numeric_limits<int>::min();
    EXPECT_THROW(calculator.Subtract(min, 1), std::overflow_error);
}

TEST_F(SimpleCalculatorTest, Multiply_Overflow_Throws)
{
    int max = std::numeric_limits<int>::max();
    EXPECT_THROW(calculator.Multiply(max, 2), std::overflow_error);
}

TEST_F(SimpleCalculatorTest, Divide_BoundaryValues)
{
    int max = std::numeric_limits<int>::max();
    int min = std::numeric_limits<int>::min();
    EXPECT_CALL(mockHistory, AddEntry(std::to_string(max) + " / 1 = " + std::to_string(max)));
    EXPECT_EQ(calculator.Divide(max, 1), max);

    EXPECT_CALL(mockHistory, AddEntry(std::to_string(min) + " / 1 = " + std::to_string(min)));
    EXPECT_EQ(calculator.Divide(min, 1), min);
}

TEST_F(SimpleCalculatorTest, SetHistory_MultipleCalls)
{
    MockHistory history1;
    MockHistory history2;
    MockHistory history3;

    InSequence seq;
    EXPECT_CALL(history1, AddEntry("2 + 2 = 4"));
    EXPECT_CALL(history2, AddEntry("3 + 3 = 6"));
    EXPECT_CALL(history3, AddEntry("4 + 4 = 8"));

    calculator.SetHistory(history1);
    calculator.Add(2, 2);
    calculator.SetHistory(history2);
    calculator.Add(3, 3);
    calculator.SetHistory(history3);
    calculator.Add(4, 4);
}

TEST(InMemoryHistoryTest, GetLastOperations_InvalidCount)
{
    InMemoryHistory history;
    history.AddEntry("1 + 1 = 2");

    auto operations = history.GetLastOperations(static_cast<size_t>(-1));
    ASSERT_EQ(operations.size(), 1);
    EXPECT_EQ(operations[0], "1 + 1 = 2");

    operations = history.GetLastOperations(std::numeric_limits<size_t>::max());
    ASSERT_EQ(operations.size(), 1);
    EXPECT_EQ(operations[0], "1 + 1 = 2");
}

TEST_F(SimpleCalculatorTest, Divide_ZeroNumerator)
{
    EXPECT_CALL(mockHistory, AddEntry("0 / 5 = 0"));
    EXPECT_EQ(calculator.Divide(0, 5), 0);
}

TEST(MockCalculatorTest, CalculatorMock_MultipleOperations)
{
    InMemoryHistory history;
    MockCalculator mockCalculator;

    InSequence seq;
    EXPECT_CALL(mockCalculator, SetHistory(_));
    EXPECT_CALL(mockCalculator, Add(2, 2)).WillOnce(Return(4));
    EXPECT_CALL(mockCalculator, Subtract(5, 3)).WillOnce(Return(2));

    mockCalculator.SetHistory(history);
    mockCalculator.Add(2, 2);
    mockCalculator.Subtract(5, 3);

    history.AddEntry("2 + 2 = 4");
    history.AddEntry("5 - 3 = 2");

    auto operations = history.GetLastOperations(2);
    ASSERT_EQ(operations.size(), 2);
    EXPECT_EQ(operations[0], "2 + 2 = 4");
    EXPECT_EQ(operations[1], "5 - 3 = 2");
}

TEST(InMemoryHistoryTest, AddEntry_EmptyString)
{
    InMemoryHistory history;
    history.AddEntry("");

    auto operations = history.GetLastOperations(1);
    ASSERT_EQ(operations.size(), 1);
    EXPECT_EQ(operations[0], "");
}

TEST(InMemoryHistoryTest, GetLastOperations_SingleEntryLargeCount)
{
    InMemoryHistory history;
    history.AddEntry("1 + 1 = 2");

    auto operations = history.GetLastOperations(100);
    ASSERT_EQ(operations.size(), 1);
    EXPECT_EQ(operations[0], "1 + 1 = 2");
}

}
