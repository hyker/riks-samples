#pragma once

#include "mqtt_client.hpp"

#include "hyker/rikskit.hpp"

// Riks wrapper for MQTTClient
struct RiksMQTTClient {
    RiksMQTTClient(
        const std::string& uid,
        const std::string& password,
        const std::string& host,
        uint16_t port,
        const std::string& config);

    void subscribe(const std::string& topic_name, std::function<void(std::string)> callback);
    void publish(const std::string& topic_name, const std::string& contents);

private:
    MQTTClient mqtt_client;
    hyker::riks::RiksKit rikskit;

    bool allowed_for_key(const char* uid, const char* key_space, const char* key_id);
    hyker::riks::Whitelist init_whitelist();
};