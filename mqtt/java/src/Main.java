import io.moquette.server.Server;

import java.io.IOException;
import java.util.Arrays;
import java.util.Properties;
import java.util.UUID;
import java.security.cert.CertificateException;

class Main {

    public static void main(String[] args) throws RiksMQTTClient.RiksMQTTException, InterruptedException, IOException {
        final String mqttHost = "localhost";
        final int mqttPort = 1234;
        final String password = "password";
        final String config = "../../default.config";
        final String topicName = "Secret channel";

        // Start MQTT server
        final Server mqttServer = new Server();
        Properties properties = new Properties();
        properties.setProperty("host", "localhost");
        properties.setProperty("port", Integer.toString(mqttPort));
        mqttServer.startServer(properties);
 
        // Start MQTT clients
        final RiksMQTTClient riksMQTTClient1 = new RiksMQTTClient(randomUID(), password, mqttHost, mqttPort, config);
        final RiksMQTTClient riksMQTTClient2 = new RiksMQTTClient(randomUID(), password, mqttHost, mqttPort, config);

        // Subscribe to topic
        riksMQTTClient2.subscribe(topicName, (String content) -> {
            System.out.println(content);
        });

        // Publish to topic
        while (true) {
            riksMQTTClient1.publish(topicName, String.format("Hello world! (%d)", System.currentTimeMillis()));
            Thread.sleep(1000);
        }
    }

    private static String randomUID() {
        return UUID.randomUUID().toString();
    }

}