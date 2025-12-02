#include "TradingMicroservice.h"
#include <pistache/endpoint.h>
#include <iostream>

int main() {
    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));
    TradingMicroservice microservice(addr);
    
    std::cout << "Starting Trading Microservice on port 9080..." << std::endl;
    microservice.init(2); // Initialize with 2 threads
    microservice.start();

    return 0;
}

