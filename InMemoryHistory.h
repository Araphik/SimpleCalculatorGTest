// (C) PROTEI protei.com

#pragma once

#include "IHistory.h"

namespace calc
{

class InMemoryHistory : public IHistory
{
public:
    InMemoryHistory() = default;

    void AddEntry(const std::string& operation) override;
    std::vector<std::string> GetLastOperations(size_t count) const override;

private:
    std::vector<std::string> m_operations;
};

} // namespace calc