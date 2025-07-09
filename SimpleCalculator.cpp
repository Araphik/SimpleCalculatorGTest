// (C) PROTEI protei.com

#include "SimpleCalculator.h"
#include "IHistory.h"

#include <stdexcept>
#include <sstream>
#include <limits>

namespace calc
{

SimpleCalculator::SimpleCalculator(IHistory& history)
    : m_history(history)
    //: m_history(&history)    // Исправление переназначения ссылки
{
}

void SimpleCalculator::SetHistory(IHistory& history)
{
    m_history = history;
    //m_history = &history;  // Исправление переназначения ссылки
}

int SimpleCalculator::Add(int a, int b)
{
    //Исправление переполнения
    /*
    if (b > 0 && a > std::numeric_limits<int>::max() - b) {
        throw std::overflow_error("Integer overflow in addition");
    }
    if (b < 0 && a < std::numeric_limits<int>::min() - b) {
        throw std::overflow_error("Integer underflow in addition");
    }
    */

    int result = a + b;
    LogOperation(a, "+", b, result);
    return result;
}

int SimpleCalculator::Subtract(int a, int b)
{
    //Исправление переполнения
    /*
    if (b < 0 && a > std::numeric_limits<int>::max() + b) {
        throw std::overflow_error("Integer overflow in subtraction");
    }
    if (b > 0 && a < std::numeric_limits<int>::min() + b) {
        throw std::overflow_error("Integer underflow in subtraction");
    }
    */

    int result = a - b;
    LogOperation(a, "-", b, result);
    return result;
}

int SimpleCalculator::Multiply(int a, int b)
{
    //Исправление переполнения
    /*
    if (b != 0) {
        // Проверяем, не превысит ли результат границы int
        if (a > 0 && b > 0 && a > std::numeric_limits<int>::max() / b) {
            throw std::overflow_error("Integer overflow in multiplication");
        }
        if (a < 0 && b < 0 && a < std::numeric_limits<int>::max() / b) {
            throw std::overflow_error("Integer overflow in multiplication");
        }
        if (a > 0 && b < 0 && b < std::numeric_limits<int>::min() / a) {
            throw std::overflow_error("Integer underflow in multiplication");
        }
        if (a < 0 && b > 0 && a < std::numeric_limits<int>::min() / b) {
            throw std::overflow_error("Integer underflow in multiplication");
        }
    }
    */

    int result = a * b;
    LogOperation(a, "*", b, result);
    return result;
}

int SimpleCalculator::Divide(int a, int b)
{
    // добавлена проверка на деление на ноль, поскольку вызывало core dumped
    if (b == 0) {
        throw std::invalid_argument("Division by zero"); 
    }

    int result = a / b;
    LogOperation(a, "/", b, result);
    return result;
}

void SimpleCalculator::LogOperation(int a, const char* op, int b, int result)
{
    std::ostringstream oss;
    oss << a << " " << op << " " << b << " = " << result;
    m_history.AddEntry(oss.str());
    //m_history->AddEntry(oss.str());  // исправление переназначения ссылки
}

} // namespace calc
