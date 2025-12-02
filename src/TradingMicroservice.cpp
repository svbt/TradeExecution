#include "TradingMicroservice.h"
#include <pistache/http.h>
#include <pistache/router.h>
#include <nlohmann/json.hpp>
#include <iostream>

namespace {
    using json = nlohmann::json;
}

TradingMicroservice::TradingMicroservice(Pistache::Address addr) 
    : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr)) {}

void TradingMicroservice::init(size_t thr) {
    auto opts = Pistache::Http::Endpoint::options().threads(thr);
    httpEndpoint->init(opts);
    setupRoutes();
}

void TradingMicroservice::start() {
    httpEndpoint->serve();
}

void TradingMicroservice::setupRoutes() {
    Pistache::Rest::Router router;
    Pistache::Rest::Routes::Post(router, "/trade/market", Pistache::Rest::Routes::bind(&TradingMicroservice::handleTradeRequest, this));
    httpEndpoint->setHandler(router.handler());
}

void TradingMicroservice::handleTradeRequest(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    try {
        auto req_json = json::parse(request.body());
        std::string symbol = req_json.at("symbol").get<std::string>();
        std::string side_str = req_json.at("side").get<std::string>();
        int quantity = req_json.at("quantity").get<int>();

        OrderSide side = (side_str == "BUY") ? OrderSide::BUY : OrderSide::SELL;

        Order executed_order = executionEngine.executeMarketOrder(symbol, side, quantity);

        json res_json = {
            {"order_id", executed_order.order_id},
            {"symbol", executed_order.symbol},
            {"side", side_str},
            {"quantity", executed_order.quantity},
            {"executed_price", executed_order.executed_price},
            {"timestamp", executed_order.timestamp}
        };

        response.headers().add<Pistache::Http::Header::ContentType>(Pistache::Http::Mime::MediaType("application/json"));
        response.send(Pistache::Http::Code::Ok, res_json.dump());

    } catch (const json::exception& e) {
        response.send(Pistache::Http::Code::Bad_Request, "Invalid JSON payload.");
    } catch (const std::exception& e) {
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());
    }
}

