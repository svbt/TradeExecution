#include "TradingMicroservice.h"
#include "KafkaConsumer.h"  // New
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>

std::atomic<bool> g_shutdown{false};

int main() 
{
    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));
    TradingMicroservice microservice(addr);

    // Start Kafka consumer
    auto kafkaConsumer = std::make_unique<KafkaConsumer>();
    kafkaConsumer->start();

    std::cout << "Starting Trading Microservice on port 9080 with Kafka consumer..." << std::endl;

    // Start HTTP in threads (non-blocking)
    microservice.init(2);

    // Separate thread for Quantum dispatcher (drives coroutines)
    std::thread quantumThread([]() {
        quantum::run();  // Blocks until shutdown
    });

    // Wait for SIGINT or similar (in production, use signal handler)
    microservice.start();  // Blocks main thread on HTTP serve

    // On shutdown
    g_shutdown = true;
    kafkaConsumer->stop();
    quantumThread.join();  // Wait for coroutines to finish

    return 0;
}