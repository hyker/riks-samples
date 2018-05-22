const RiksMQTTClient = require('./riks-mqtt-client.js');
const aedes = require('aedes')();

const randomUID = () => Math.random().toString(36).slice(-16);
const mqttHost = "localhost";
const mqttPort = 1234;
const config = "../../default.config";
const topicName = "Secret channel";

// Start MQTT server
const mqttServer = require('net').createServer(aedes.handle);
mqttServer.listen(mqttPort, () => console.log('server listening on port', mqttPort));

// Start MQTT clients
const riksMQTTClient1 = new RiksMQTTClient(randomUID(), "password", mqttHost, mqttPort, config);
const riksMQTTClient2 = new RiksMQTTClient(randomUID(), "password", mqttHost, mqttPort, config);

// Subscribe to topic
riksMQTTClient2.subscribe(topicName, (content) => {
    console.log(content);
});

// Publish to topic
(function pulse() {
  const timestamp = Date.now();
  riksMQTTClient1.publish(topicName, "Hello world! (" + timestamp + ")");
  setTimeout(pulse, 1000);
})();