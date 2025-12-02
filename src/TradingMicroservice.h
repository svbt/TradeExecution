#pragma once

#include <pistache/endpoint.h>
#include "ExecutionEngine.h"

namespace Pistache {
    class Address;
    namespace Rest {
        class Request;
    }
    namespace Http {
        class ResponseWriter;
        class Endpoint;
    }
}

class TradingMicroservice {
public:
    explicit TradingMicroservice(Pistache::Address addr);
    void init(size_t thr);
    void start();

private:
    void setupRoutes();
    void handleTradeRequest(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

    std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint;
    ExecutionEngine executionEngine;
};

