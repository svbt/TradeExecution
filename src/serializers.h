#pragma once
#include <corokafka/corokafka.h>
#include <vector>
#include <string>
#include "TradeRequest.h"

namespace Bloomberg::corokafka {

// Serializer/Deserializer for key: std::string (simple empty key for trades)
template<> struct Serialize<std::string> {
    std::vector<uint8_t> operator()(const std::string& s) {
        return {s.begin(), s.end()};
    }
};

template<> struct Deserialize<std::string> {
    std::string operator()(const cppkafka::TopicPartition&, const cppkafka::Buffer& buf) {
        return std::string(buf.begin(), buf.end());
    }
};

// Serializer/Deserializer for payload: TradeRequest (JSON via nlohmann)
template<> struct Serialize<TradeRequest> {
    std::vector<uint8_t> operator()(const TradeRequest& req) {
        nlohmann::json j = req;
        std::string json_str = j.dump();
        return {json_str.begin(), json_str.end()};
    }
};

template<> struct Deserialize<TradeRequest> {
    TradeRequest operator()(const cppkafka::TopicPartition&, const cppkafka::Buffer& buf) {
        std::string json_str(buf.begin(), buf.end());
        nlohmann::json j = nlohmann::json::parse(json_str);
        TradeRequest req;
        j.get_to(req);
        return req;
    }
};

// No headers for simplicity
template<> struct Serialize<void> { std::vector<uint8_t> operator()(const void&) { return {}; } };
template<> struct Deserialize<void> { void operator()(const cppkafka::TopicPartition&, const cppkafka::Buffer&) {} };

}  // namespace Bloomberg::corokafka