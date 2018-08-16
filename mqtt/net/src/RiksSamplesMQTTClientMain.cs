using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

class RiksSamplesMQTTClientMain
{

    static void Main(string[] args)
    {
        Task.Run(async () => {
            var mqttHost = "mqtt.dev.v2.hykr.io";
            var mqttPort = 1883;
            var configFile = "../../default.config";

            try
            {
                // Create and start the consumer
                var consumer = await RiksMQTTClient.Connect(GenerateRandomUID(), "password", mqttHost, mqttPort, configFile);

                // Create and start the producer
                var producer = await RiksMQTTClient.Connect(GenerateRandomUID(), "password", mqttHost, mqttPort, configFile);
                
                // Subscribe to topic "SuperSecretSpeakingClock"
                consumer.Subscribe("SuperSecretSpeakingClock", Console.WriteLine);

                // Publish current time every second
                while (true)
                {
                    var timestamp = DateTimeOffset.Now.ToUnixTimeMilliseconds();
                    producer.Publish("SuperSecretSpeakingClock", String.Format("The time is: {0}", timestamp));
                    Thread.Sleep(1000);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }).GetAwaiter().GetResult();
    }

    private static string GenerateRandomUID()
    {
        var chars = "abcdefghijklmnopqrstuvwxyz0123456789";
        var random = new Random();
        return new string(chars.Select(c => chars[random.Next(chars.Length)]).Take(16).ToArray());
    }

}
