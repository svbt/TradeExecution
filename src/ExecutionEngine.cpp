#include "ExecutionEngine.h"
#include <iostream>

ExecutionEngine::ExecutionEngine() {
    // Hard-coded market prices for simulation
    market_prices["AAPL"] = 180.50;
    market_prices["MSFT"] = 420.75;
    market_prices["GOOG"] = 140.25;
}

Order ExecutionEngine::executeMarketOrder(const std::string& symbol, OrderSide side, int quantity) {
    if (market_prices.find(symbol) == market_prices.end()) {
        throw std::runtime_error("Symbol not found");
    }

    // In a real-world system, this would interact with a real exchange API
    double execution_price = market_prices[symbol];

    Order executed_order;
    executed_order.order_id = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    executed_order.symbol = symbol;
    executed_order.side = side;
    executed_order.quantity = quantity;
    executed_order.executed_price = execution_price;
    executed_order.timestamp = std::to_string(std::time(nullptr));

    std::cout << "Executed market order: " 
              << orderSideToString(executed_order.side) << " "
              << executed_order.quantity << " " 
              << executed_order.symbol << " at $" 
              << execution_price << std::endl;

    return executed_order;
}

