#include "riks_mqtt_client.hpp"

RiksMQTTClient::RiksMQTTClient(
    const std::string& uid,
    const std::string& password,
    const std::string& host,
    uint16_t port,
    const std::string& config) :

    mqtt_client (uid.c_str(), host.c_str(), port),
    rikskit (uid, password, init_whitelist(), config) {
}

void RiksMQTTClient::subscribe(const std::string& topic_name, std::function<void(std::string)> callback) {
    mqtt_client.subscribe(topic_name, [this, callback](std::string contents) {
        rikskit.decrypt(contents).then([callback](const auto& decrypted_message) {
            callback(decrypted_message.secret_data);
        });
    });
}

void RiksMQTTClient::publish(const std::string& topic_name, const std::string& contents) {
    rikskit.encrypt({contents, {/*immutable data*/}, {/*mutable data*/}}, topic_name)
        .then([this, topic_name](const auto& encrypted_message) {
            mqtt_client.publish(topic_name, encrypted_message);
        });
}

bool RiksMQTTClient::allowed_for_key(const char* uid, const char* key_space, const char* key_id) {
    std::cout << uid << " requested access to topic " << key_space << ". Granting access.\n";
    return true;
}

hyker::riks::Whitelist RiksMQTTClient::init_whitelist() {
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