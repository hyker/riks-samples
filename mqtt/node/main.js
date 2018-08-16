const RiksMQTTClient = require('./riks-mqtt-client.js');

const randomUID = () => Math.random().toString(36).slice(-16);

const mqttHost = "mqtt.dev.v2.hykr.io";
const mqttPort = 1883;
const config = "../../default.config";

// Start MQTT clients
const riksMQTTClient1 = new RiksMQTTClient(randomUID(), "password", mqttHost, mqttPort, config);
const riksMQTTClient2 = new RiksMQTTClient(randomUID(), "password", mqttHost, mqttPort, config);

const topicName = "SuperSecretSpeakingClock";

// Subscribe to topic
riksMQTTClient2.subscribe(topicName, (content) => {
    console.log(content);
});

// Publish to topic
(function pulse() {
  const timestamp = Date.now();
  riksMQTTClient1.publish(topicName, "The time is: " + timestamp);
  setTimeout(pulse, 1000);
})();