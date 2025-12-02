#pragma once

#include "Order.h"
#include <map>
#include <string>

class ExecutionEngine {
private:
    // Hard-coded market data for demonstration
    std::map<std::string, double> market_prices;

public:
    ExecutionEngine();
    Order executeMarketOrder(const std::string& symbol, OrderSide side, int quantity);
};

