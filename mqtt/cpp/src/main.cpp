#include "mqtt_client.hpp"
#include "mqtt_server.hpp"

#include "hyker/util/random.hpp"
#include "hyker/rikskit.hpp"

#include <chrono>
#include <iostream>
#include <thread>

// Riks wrapper for MQTTClient
struct RiksMQTTClient {
    RiksMQTTClient(
        const std::string& uid,
        const std::string& password,
        const std::string& host,
        uint16_t port,
        const std::string& config) :

        mqtt_client (uid.c_str(), host.c_str(), port),
        rikskit (uid, password, init_whitelist(), config.c_str()) {
    }

    void subscribe(const std::string& topic_name, std::function<void(std::string)> callback) {
        mqtt_client.subscribe(topic_name, [this, callback](std::string contents) {
            rikskit.decrypt(contents).then([callback](const auto& decrypted_message) {
                callback(decrypted_message.secret_data);
            });
        });
    }

    void publish(const std::string& topic_name, const std::string& contents) {
        rikskit.encrypt({contents, {/*immutable data*/}, {/*mutable data*/}}, topic_name)
            .then([this, topic_name](const auto& encrypted_message) {
                mqtt_client.publish(topic_name, encrypted_message);
            });
    }

private:
    MQTTClient mqtt_client;
    hyker::riks::RiksKit rikskit;

    bool allowed_for_key(const char* uid, const char* key_space, const char* key_id) {
        std::cout << uid << " requested access to topic " << key_space << ". Granting access.\n";
        return true;
    }

    hyker::riks::Whitelist init_whitelist() {
        return {
            std::bind(
                &RiksMQTTClient::allowed_for_key,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
            ),
            {} // new_key
        };
    }
};

int main() {
    const uint16_t port = 1234;
    MQTTServer mqtt_server{port};

    const auto config = "../../../default.config";
    RiksMQTTClient riks_mqtt_client_1{hyker::util::random_string(16), "password", "localhost", port, config};
    RiksMQTTClient riks_mqtt_client_2{hyker::util::random_string(16), "password", "localhost", port, config};

    const auto topic_name = "Secret channel";

    riks_mqtt_client_2.subscribe(topic_name, [](std::string content) {
        std::cout << content << '\n';
    });

    while (true) {
        riks_mqtt_client_1.publish(topic_name, "hello world");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}