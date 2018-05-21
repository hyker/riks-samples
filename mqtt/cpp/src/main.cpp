#include "mqtt_client.hpp"
#include "mqtt_server.hpp"

#include "hyker/util/random.hpp"
#include "hyker/rikskit.hpp"
#include "hyker/outbox.hpp"

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
        rikskit (uid, password, init_whitelist(), config) {
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
    const auto mqtt_host = "localhost";
    const uint16_t mqtt_port = 1234;
    const auto config = "../../../default.config";

    MQTTServer mqtt_server{mqtt_port};
    RiksMQTTClient riks_mqtt_client_1{hyker::util::random_string(16).c_str(), "password", mqtt_host, mqtt_port, config};
    RiksMQTTClient riks_mqtt_client_2{hyker::util::random_string(16).c_str(), "password", mqtt_host, mqtt_port, config};

    const auto topic_name = "Secret channel";

    riks_mqtt_client_1.subscribe(topic_name, [](std::string content) {
        std::cout << content << '\n';
    });

    while (true) {
        const auto now = std::chrono::system_clock::now().time_since_epoch();;
        const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

        riks_mqtt_client_1.publish(topic_name, "Hello world! (" + std::to_string(timestamp) + ")");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}