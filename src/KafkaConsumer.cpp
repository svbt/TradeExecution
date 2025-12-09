#include "KafkaConsumer.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include "serializers.h"  // Include after TradeRequest.h

// Static members
std::mutex KafkaConsumer::s_mutex;
std::deque<TradeTopic::receivedMessage> KafkaConsumer::s_messages;
ExecutionEngine KafkaConsumer::s_executionEngine;
std::atomic<bool> KafkaConsumer::s_running{false};

KafkaConsumer::KafkaConsumer(const std::string& brokers) {
    // Consumer config
    std::initializer_list<cppkafka::ConfigurationOption> consumerOpts = {
        {"metadata.broker.list", brokers},
        {"group.id", "trade-execution-group"},
        {"enable.auto.commit", true},
        {"auto.offset.reset", "earliest"}
    };

    m_cfg = corokafka::ConsumerConfiguration(TradeTopic{}, consumerOpts, {}, receiverCallback);
    m_cfg.setLogCallback([](const corokafka::Metadata&, cppkafka::LogLevel level, const std::string&, const std::string& msg) {
        std::cout << "[Kafka Log " << (int)level << "] " << msg << std::endl;
    });

    corokafka::ConnectorConfiguration connCfg({{"internal.connector.poll.interval.ms", 100}});
    corokafka::ConfigurationBuilder builder;
    builder.add(m_cfg).add(connCfg);

    m_connector = corokafka::Connector(std::move(builder));
}

KafkaConsumer::~KafkaConsumer() {
    stop();
}

void KafkaConsumer::start() {
    if (s_running) return;
    s_running = true;

    // Subscribe to topic
    m_connector.consumer().subscribe({TradeTopic::name()});

    // Launch coroutine processor
    quantum::Go(messageProcessor);

    std::cout << "Kafka consumer started for topic 'execute-trade'" << std::endl;
}

void KafkaConsumer::stop() {
    s_running = false;
    m_connector.consumer().unsubscribe();
    std::cout << "Kafka consumer stopped" << std::endl;
}

void KafkaConsumer::receiverCallback(TradeTopic::receivedMessage msg) {
    if (!msg || msg.getError()) {
        if (msg.getError()) {
            std::cerr << "Consumer error: " << msg.getError()->to_string() << std::endl;
        }
        return;
    }

    {
        std::lock_guard<std::mutex> lk(s_mutex);
        s_messages.emplace_back(std::move(msg));
    }
}

quantum::Task<> KafkaConsumer::messageProcessor() {
    while (s_running) {
        TradeTopic::receivedMessage msg;
        {
            std::lock_guard<std::mutex> lk(s_mutex);
            if (s_messages.empty()) {
                co_await quantum::Sleep(std::chrono::milliseconds(10));
                continue;
            }
            msg = std::move(s_messages.front());
            s_messages.pop_front();
        }

        try {
            // Deserialize (handled by CoroKafka)
            const TradeRequest& req = msg.getPayload();
            Order order = req.toOrder();
            Order executed = s_executionEngine.executeMarketOrder(order.symbol, order.side, order.quantity);

            // Log result (extend to produce back if needed)
            nlohmann::json res_json = {
                {"order_id", executed.order_id},
                {"symbol", executed.symbol},
                {"side", req.side},
                {"quantity", executed.quantity},
                {"executed_price", executed.executed_price},
                {"timestamp", executed.timestamp}
            };
            std::cout << "[Kafka Executed] " << res_json.dump() << std::endl;

            // Commit offset
            msg.commit();
        } catch (const std::exception& e) {
            std::cerr << "[Kafka Process Error] " << e.what() << std::endl;
        }

        co_await quantum::Sleep(std::chrono::milliseconds(0));  // Yield
    }
}