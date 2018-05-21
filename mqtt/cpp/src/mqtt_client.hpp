#pragma once

#include "mqtt_client_cpp.hpp"

struct MQTTClient {
    MQTTClient( const char* client_id, const char* server_host, uint16_t server_port);

    void subscribe(const std::string& topic_name, std::function<void(std::string)> callback);
    void publish(const std::string& topic_name, const std::string& contents);

private:
    boost::asio::io_service io_service;
    std::shared_ptr<mqtt::client<mqtt::tcp_endpoint<boost::asio::ip::tcp::socket, boost::asio::io_service::strand>>> mqtt_client;
    std::map<std::string, std::function<void(std::string)>> subscriptions;
    bool connected = false;

    void when_connected(const std::function<void()>& function);
};