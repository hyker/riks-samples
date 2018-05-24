import io.hyker.cryptobox.PublicKeyLookupException;
import io.hyker.riks.Message;
import io.hyker.riks.RiksKit;
import io.hyker.riks.Whitelist;
import io.hyker.Future;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.io.IOException;
import java.util.function.Consumer;
import java.util.UUID;
import java.util.Map;
import java.util.HashMap;
import java.security.cert.CertificateException;

public class RiksMQTTClient {

    private final MqttClient mqttClient;
    private final RiksKit riksKit;
    private final Map<String, Consumer<MqttMessage>> subscriptions = new HashMap<>();

    RiksMQTTClient(String uid, String password, String host, int port, String config) throws RiksMQTTException {
        try {
            // Initialize MQTT client
            this.mqttClient = new MqttClient(String.format("tcp://%s:%d", host, port), uid, new MemoryPersistence());
            MqttConnectOptions mqttConnectOptions = new MqttConnectOptions();
            mqttConnectOptions.setCleanSession(true);
            this.mqttClient.connect(mqttConnectOptions);
            this.mqttClient.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                }
                @Override
                public void messageArrived(String messageTopic, MqttMessage message) {
                    for (String topic : subscriptions.keySet()) {
                        if (messageTopic.equals(topic)) {
                            subscriptions.get(topic).accept(message);
                        }
                    }
                }
                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                }
            });

            // Initialize RiksKit
            final Whitelist whitelist = new Whitelist() {
                @Override
                public Future<Boolean> allowedForKey(String uid, String keySpace, String keyID) {
                    Future<Boolean> future = new Future<>();

                    // Let everyone have access
                    future.set(true);

                    return future;
                }
                @Override
                public void newKey(String keySpace, String keyID) {
                }
            };
            this.riksKit = new RiksKit(uid, password, whitelist, config);
        } catch (IOException | MqttException | PublicKeyLookupException | CertificateException e) {
            throw new RiksMQTTException(e);
        }
    }

    public void subscribe(String topic, Consumer<String> callback) throws RiksMQTTException {
        try {
            mqttClient.subscribe(topic);
            subscriptions.put(topic, (MqttMessage mqttMessage) -> {
                riksKit.decrypt(mqttMessage.getPayload()).then((Message riksMessage) -> {
                    callback.accept(new String(riksMessage.getSecretData()));
                });
            });
        } catch (MqttException e) {
            throw new RiksMQTTException(e);
        }
    }

    public void publish(String topic, String content) {
        Message riksMessage = new Message(content.getBytes());
        riksKit.encrypt(riksMessage, topic).then((byte[] encryptedContent) -> {
            MqttMessage mqttMessage = new MqttMessage(encryptedContent);
            mqttMessage.setQos(2);
            try {
                mqttClient.publish(topic, mqttMessage);
            } catch (MqttException e) {
                System.err.println(e);
            }
        });
    }

    public class RiksMQTTException extends Exception {
        RiksMQTTException(Exception e) {
            super(e);
        }
    }

}