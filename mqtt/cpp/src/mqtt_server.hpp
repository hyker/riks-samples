#pragma once

#include "mqtt_server_cpp.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

struct sub_con {
    sub_con(
        const std::string& topic,
        const std::shared_ptr<mqtt::server<>::endpoint_t>& con,
        uint8_t qos) :

        topic (topic),
        con (con),
        qos (qos) {
    }
    
    std::string topic;
    std::shared_ptr<mqtt::server<>::endpoint_t> con;
    uint8_t qos;
};

struct tag_topic {};
struct tag_con {};

using mi_sub_con = boost::multi_index::multi_index_container<
    sub_con,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<tag_topic>,
            BOOST_MULTI_INDEX_MEMBER(sub_con, std::string, topic)
        >,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<tag_con>,
            BOOST_MULTI_INDEX_MEMBER(sub_con, std::shared_ptr<mqtt::server<>::endpoint_t>, con)
        >
    >
>;

struct MQTTServer {
    MQTTServer(uint16_t port);

private:
    boost::asio::io_service io_service;
    mqtt::server<> mqtt_server;
    std::set<std::shared_ptr<mqtt::server<>::endpoint_t>> connections;
    mi_sub_con subs;

    mqtt::server<> init_mqtt_server(uint16_t port);
    void close_proc(const std::shared_ptr<mqtt::server<>::endpoint_t>& con);
};