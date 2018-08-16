import java.io.IOException;
import java.util.Arrays;
import java.util.Properties;
import java.util.UUID;
import java.security.cert.CertificateException;

class Main {

    public static void main(String[] args) throws RiksMQTTClient.RiksMQTTException, InterruptedException, IOException {
        final String mqttHost = "mqtt.dev.v2.hykr.io";
        final int mqttPort = 1883;
        final String config = "../../default.config";

        // Start MQTT clients
        final RiksMQTTClient riksMQTTClient1 = new RiksMQTTClient(randomUID(), "password", mqttHost, mqttPort, config);
        final RiksMQTTClient riksMQTTClient2 = new RiksMQTTClient(randomUID(), "password", mqttHost, mqttPort, config);

        final String topicName = "SuperSecretSpeakingClock";

        // Subscribe to topic
        riksMQTTClient2.subscribe(topicName, (String content) -> {
            System.out.println(content);
        });

        // Publish to topic
        while (true) {
            riksMQTTClient1.publish(topicName, String.format("The time is: %d", System.currentTimeMillis()));
            Thread.sleep(1000);
        }
    }

    private static String randomUID() {
        return UUID.randomUUID().toString();
    }

}