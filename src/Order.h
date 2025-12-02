#pragma once

#include <string>
#include <chrono>

enum class OrderSide { BUY, SELL };

struct Order {
    std::string order_id;
    std::string symbol;
    OrderSide side;
    int quantity;
    double executed_price;
    std::string timestamp;
};

// Helper function to convert OrderSide enum to a string
inline std::string orderSideToString(OrderSide side) {
    if (side == OrderSide::BUY) {
        return "BUY";
    } else {
        return "SELL";
    }
}