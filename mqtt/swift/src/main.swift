import Foundation

var mqttHost = "localhost"
var mqttPort = 1234
var configFile = "../../../default.config"

// Create and start the consumer
var consumer = try RiksMQTTClient(
    uid: UUID().uuidString,
    password: "password",
    host: mqttHost,
    port: mqttPort,
    config: configFile
)

// Create and start the producer
var producer = try RiksMQTTClient(
    uid: UUID().uuidString,
    password: "password",
    host: mqttHost,
    port: mqttPort,
    config: configFile
)

// Subscribe to topic "SuperSecretSpeakingClock"
consumer.subscribe(topic: "SuperSecretSpeakingClock", callback: {(message: String) in print(message)})

// Publish current time every second
while (true) {
    let timestamp = Date().timeIntervalSince1970 * 1000
    producer.publish(topic: "SuperSecretSpeakingClock", message: String(format: "%f", timestamp))
    sleep(1)
}
