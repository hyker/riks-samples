import Foundation

var mqttHost = "localhost"
var mqttPort = 1234
var configFile = "../../../default.config"

var consumer = RiksMQTTClient(
    uid: UUID().uuidString,
    password: "password",
    host: mqttHost,
    port: mqttPort,
    config: configFile
)
