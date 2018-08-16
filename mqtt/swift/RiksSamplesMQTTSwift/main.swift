import Foundation

let mqttHost = "mqtt.dev.v2.hykr.io"
let mqttPort: UInt16 = 1883
let configFile = "../../../default.config"

// Create and start the consumer
let consumer = try RiksMQTTClient(
    uid: UUID().uuidString,
    password: "password",
    host: mqttHost,
    port: mqttPort,
    config: configFile
)

// Create and start the producer
let producer = try RiksMQTTClient(
    uid: UUID().uuidString,
    password: "password",
    host: mqttHost,
    port: mqttPort,
    config: configFile
)

let topicName = "SuperSecretSpeakingClock" + UUID().uuidString

// Subscribe to topic
consumer.subscribe(topic: topicName, callback: {(message: String) in print(message)})

// Publish current time every second
while (true) {
    let timestamp = Date().timeIntervalSince1970 * 1000
    producer.publish(topic: topicName, message: String(format: "%f", timestamp))
    sleep(1)
}
