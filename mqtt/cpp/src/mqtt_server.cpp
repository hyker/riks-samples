#include "mqtt_server.hpp"

#include <iostream>
#include <thread>

MQTTServer::MQTTServer(uint16_t port) :
    mqtt_server (init_mqtt_server(port)) {

    mqtt_server.set_error_handler([](const auto& error_code) {
        std::cerr << "[MQTT Server] error: " << error_code.message() << '\n';
    });

    mqtt_server.set_accept_handler([&](mqtt::server<>::endpoint_t& endpoint) {
        auto sp = endpoint.shared_from_this();

        endpoint.start_session([&, sp](const auto& error_code) {
            mqtt_server.close();
        });

        endpoint.set_close_handler([&]() {
            this->close_proc(endpoint.shared_from_this());
        });

        endpoint.set_error_handler([&](const auto& error_code) {
            std::cerr << "[MQTT Server] error: " << error_code.message() << '\n';
            this->close_proc(endpoint.shared_from_this());
        });

        endpoint.set_connect_handler([&](
            const auto& client_id,
            const auto& username,
            const auto& password,
            auto,
            bool clean_session,
            uint16_t) {

            connections.insert(endpoint.shared_from_this());
            endpoint.connack(false, mqtt::connect_return_code::accepted);
            return true;
        });

        endpoint.set_disconnect_handler([&]() {
            this->close_proc(endpoint.shared_from_this());
        });

        endpoint.set_publish_handler([&](uint8_t header, auto packet_id, auto topic_name, auto contents) {
            uint8_t qos = mqtt::publish::get_qos(header);
            bool retain = mqtt::publish::is_retain(header);
            auto const& idx = subs.get<tag_topic>();
            auto r = idx.equal_range(topic_name);
            for (; r.first != r.second; ++r.first) {
                r.first->con->publish(
                    topic_name,
                    contents,
                    std::min(r.first->qos, qos),
                    retain
                );
            }
            return true;
        });

        endpoint.set_subscribe_handler([&](uint16_t packet_id, auto entries) {
            std::vector<uint8_t> res;
            res.reserve(entries.size());
            for (auto const& e : entries) {
                std::string const& topic = std::get<0>(e);
                uint8_t qos = std::get<1>(e);
                res.emplace_back(qos);
                subs.emplace(topic, endpoint.shared_from_this(), qos);
            }
            endpoint.suback(packet_id, res);
            return true;
        });

        endpoint.set_unsubscribe_handler([&](uint16_t packet_id, auto topics) {
            for (auto const& topic : topics) {
                subs.erase(topic);
            }
            endpoint.unsuback(packet_id);
            return true;
        });
    });

    mqtt_server.listen();

    std::thread{[this]() {
        while (true) io_service.run();
    }}.detach();
}

mqtt::server<> MQTTServer::init_mqtt_server(uint16_t port) {
    return {
        boost::asio::ip::tcp::endpoint{
            boost::asio::ip::tcp::v4(),
            port
        },
        io_service
    };
}

void MQTTServer::close_proc(const std::shared_ptr<mqtt::server<>::endpoint_t>& con) {
    connections.erase(con);

    auto& idx = subs.get<tag_con>();
    auto r = idx.equal_range(con);
    idx.erase(r.first, r.second);
}