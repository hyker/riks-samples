#include "mqtt_client.hpp"

#include <thread>

MQTTClient::MQTTClient(
    const char* client_id,
    const char* server_host,
    uint16_t server_port) :

    mqtt_client (mqtt::make_client(io_service, server_host, server_port)) {

    mqtt_client->set_client_id(client_id);
    mqtt_client->set_clean_session(true);
    mqtt_client->set_connack_handler([&](bool session_present, uint8_t connack_return_code) {
        connected = connack_return_code == mqtt::connect_return_code::accepted;
        return true;
    });
    mqtt_client->set_publish_handler([&](uint8_t header, auto packet_id, auto topic_name, auto contents) {
        if (subscriptions.find(topic_name) != subscriptions.end()) {
            subscriptions[topic_name](contents);
        }
        return true;
    });
    mqtt_client->connect();

    std::thread{[this]() {
        while (true) io_service.run();
    }}.detach();
}

void MQTTClient::subscribe(const std::string& topic_name, std::function<void(std::string)> callback) {
    subscriptions[topic_name] = callback;
    when_connected([=]() {
        mqtt_client->subscribe(topic_name, mqtt::qos::exactly_once);
    });
}

void MQTTClient::publish(const std::string& topic_name, const std::string& contents) {
    mqtt_client->publish_exactly_once(topic_name, contents);
}

void MQTTClient::when_connected(const std::function<void()>& function) {
    io_service.post([=]() {
        if (connected) {
            function();
        } else {
            when_connected(function);
        }
    });
}