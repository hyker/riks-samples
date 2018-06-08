package demo;

import java.util.Arrays;
import java.util.Properties;
import java.util.UUID;
import java.security.cert.CertificateException;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        try {
            final String mqttHost = "localhost";
            final int mqttPort = 1234;
            final String password = "password";
            final String config = "../../default.config";
            final String topicName = "SuperSecretSpeakingClock";

            // Start MQTT clients
            final RiksMQTTClient riksMQTTClient1 = new RiksMQTTClient(randomUID(), password, mqttHost, mqttPort, config);
            final RiksMQTTClient riksMQTTClient2 = new RiksMQTTClient(randomUID(), password, mqttHost, mqttPort, config);

            // Subscribe to topic
            riksMQTTClient2.subscribe(topicName, (String content) -> {
                System.out.println(content);
            });

            // Publish to topic
            while (true) {
                riksMQTTClient1.publish(topicName, String.format("The time is: %d", System.currentTimeMillis()));
                Thread.sleep(1000);
            }
        } catch (RiksMQTTClient.RiksMQTTException | InterruptedException e) {
            
        }
    }

    private static String randomUID() {
        return UUID.randomUUID().toString();
    }

}