package io.hyker.riks.samples.mqtt.android;

import io.hyker.Json;

import java.util.Arrays;
import java.util.Properties;
import java.util.UUID;
import java.security.cert.CertificateException;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        new Thread(() -> {
            try {
                final String mqttHost = "mqtt.dev.v2.hykr.io";
                final int mqttPort = 1883;

                // Load config
                Json config = Json.parse(readRawTextFile(R.raw.config));
                File storageDir = new File(getApplicationContext().getFilesDir(), "storage");

                // Start MQTT clients
                final RiksMQTTClient riksMQTTClient1 = new RiksMQTTClient(randomUID(), "password", mqttHost, mqttPort, config, storageDir);
                final RiksMQTTClient riksMQTTClient2 = new RiksMQTTClient(randomUID(), "password", mqttHost, mqttPort, config, storageDir);

                final String topicName = "SuperSecretSpeakingClock";
                
                // Subscribe to topic
                riksMQTTClient2.subscribe(topicName, (String content) -> {
                    showText(content);
                });

                // Publish to topic
                while (true) {
                    riksMQTTClient1.publish(topicName, String.format("The time is: %d", System.currentTimeMillis()));
                    Thread.sleep(1000);
                }
            } catch (Throwable e) {
                showText(e.getMessage());
            }
        }).start();
    }

    private void showText(String text) {
        runOnUiThread(() -> {
            TextView textView = (TextView)findViewById(R.id.clock);
            textView.setText(text);
        });
    }

    private String readRawTextFile(int resource) throws IOException {
        InputStream stream = getResources().openRawResource(resource);
        BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
        StringBuilder content = new StringBuilder();
        String line;

        while ((line = reader.readLine()) != null) {
            content.append(line);
            content.append(System.lineSeparator());
        }

        return content.toString();
    }

    private static String randomUID() {
        return UUID.randomUUID().toString();
    }

}