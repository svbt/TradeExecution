#pragma once
#include <corokafka/corokafka.h>
#include <quantum/Quantum.h>
#include <memory>
#include "ExecutionEngine.h"
#include "TradeRequest.h"

using TradeTopic = corokafka::Topic<std::string, TradeRequest>;  // Key: str, Payload: TradeRequest

class KafkaConsumer {
public:
    KafkaConsumer(const std::string& brokers = "localhost:9092");
    ~KafkaConsumer();

    void start();  // Launch coroutine consumer
    void stop();

private:
    // Receiver callback: Enqueues messages for coroutine processing
    static void receiverCallback(TradeTopic::receivedMessage msg);

    // Coroutine worker: Processes enqueued messages
    static quantum::Task<> messageProcessor();

    // Shared state
    static std::mutex s_mutex;
    static std::deque<TradeTopic::receivedMessage> s_messages;
    static ExecutionEngine s_executionEngine;
    static std::atomic<bool> s_running;

    // CoroKafka objects
    corokafka::Connector m_connector;
    corokafka::ConsumerConfiguration m_cfg;
};