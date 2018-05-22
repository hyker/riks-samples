'use strict'

const RiksKit = require('riks')
const mqtt = require('mqtt')

module.exports = class RiksMQTTClient {

  constructor(uid, password, host, port, config) {
    if (typeof uid !== 'string')
      throw 'uid must be a string'

    if (typeof password !== 'string')
      throw 'password must be a string'

    if (typeof host !== 'string')
      throw 'host must be a string'

    if (typeof port !== 'number' || port < 1 || port > 65535)
      throw 'port must be a number between 1 and 65535'
    
    if (typeof config !== 'string' && typeof config !== 'object')
      throw 'config must be a filename or an object'
    
    const whitelist = (uid, keyspace, keyID, callback) => {
      // Let everyone have access
      return true;
    };

    const newKey = (keyspace, keyID) => {
    };

    this.riksKit = new RiksKit(uid, password, whitelist, newKey, config);
    this.mqttClient = mqtt.connect({
      host: host,
      port: port
    });
  }

  subscribe(topic, callback) {
    const that = this;
    this.mqttClient.subscribe(topic)
    this.mqttClient.on('message', (messageTopic, message) => {
      if (topic === messageTopic) {
        that.riksKit.decrypt(message.toString())
          .then(callback)
          .catch(callback)
      }
    })
  }

  async publish(topic, content) {
    return this.riksKit.encrypt(content, topic)
      .then(encrypted => this.mqttClient.publish(topic, encrypted))
  }

  end() {
    this.mqttClient.end()
  }

};