#pragma once
#include <nlohman/json.hpp>
#include <string>
#include "Order.h"

struct TradeRequest
{
	std::string symbol;
	std::string side;
	int quantity;

	# JSON serialization (for Kafka payload)
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TradeRequest, symbol, side, quantity)

	# Convert to order (for execution)
	Order toOrder() const
	{
		OrderSide orderside = (side == "BUY") ? OrderSide::BUY : OrderSide::SELL;
		Order order;
		order.symbol = symbol;
		order.side = side;
		order.quantity = quantity;

		return order;
	}
};