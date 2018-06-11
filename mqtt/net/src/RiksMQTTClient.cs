using System.Collections.Generic;
using System.Threading.Tasks;

using Hyker.Riks;

using MQTTnet;
using MQTTnet.Client;

public class RiksMQTTClient
{

    public delegate void SubscriptionDelegate(string data);

    private readonly IMqttClient mqttClient;
    private readonly RiksKit riksKit;

    /// <summary> 
    ///   Sets up a connection to the MQTT server specified by <c>uid</c>, <c>host</c> and <c>port</c>.
    ///   Sets up a connection to the Hyker platform using <c>uid</c> and the settings in <c>config</c>.
    ///   Registers the UID if it does not already exist in the system.
    /// </summary>
    /// <exception cref="System.Exception">
    ///   Thrown when configuration file does not exist or contains badly formatted properties,
    ///   if the UID is already registered on another device,
    ///   or if the connection to the KDS cannot be established.
    /// </exception>
    /// <param name="uid">The unique identifier of the current device.</param>
    /// <param name="password">The password for the private key of the current device.</param>
    /// <param name="config">The file containing the Riks settings, API keys, etc. to be used.</param>
    public static async Task<RiksMQTTClient> Connect(string uid, string password, string host, int port, string config)
    {
        var mqttClient = new MqttFactory().CreateMqttClient();
        var options = new MqttClientOptionsBuilder()
            .WithClientId(uid)
            .WithTcpServer(host, port)
            .Build();
        await mqttClient.ConnectAsync(options);

        return new RiksMQTTClient(mqttClient, uid, password, config);
    }

    /// <summary> 
    ///   Sets up a connection to the MQTT server specified by <c>uid</c>, <c>host</c> and <c>port</c>.
    ///   Sets up a connection to the Hyker platform using <c>uid</c> and the settings in <c>config</c>.
    ///   Registers the UID if it does not already exist in the system.
    /// </summary>
    /// <exception cref="System.Exception">
    ///   Thrown when configuration file does not exist or contains badly formatted properties,
    ///   if the UID is already registered on another device,
    ///   or if the connection to the KDS cannot be established.
    /// </exception>
    /// <param name="uid">The unique identifier of the current device.</param>
    /// <param name="password">The password for the private key of the current device.</param>
    /// <param name="config">The dictionary containing the Riks settings, API keys, etc. to be used.</param>
    public static async Task<RiksMQTTClient> Connect(string uid, string password, string host, int port, Dictionary<string, object> config)
    {
        var mqttClient = new MqttFactory().CreateMqttClient();
        var options = new MqttClientOptionsBuilder()
            .WithClientId(uid)
            .WithTcpServer(host, port)
            .Build();
        await mqttClient.ConnectAsync(options);

        return new RiksMQTTClient(mqttClient, uid, password, config);
    }

    private RiksMQTTClient(IMqttClient mqttClient, string uid, string password, string config)
    {
        this.mqttClient = mqttClient;
        riksKit = new RiksKit(uid, password, new Whitelist(AllowedForKey, (string keySpace, string keyID) => { }), config);
    }

    private RiksMQTTClient(IMqttClient mqttClient, string uid, string password, Dictionary<string, object> config)
    {
        this.mqttClient = mqttClient;
        riksKit = new RiksKit(uid, password, new Whitelist(AllowedForKey, (string keySpace, string keyID) => { }), config);
    }

    private bool AllowedForKey(string uid, string keySpace, string keyID)
    {
        // Allow all requests
        System.Console.WriteLine(uid + " requested access to topic " + keySpace + ". Granting access.");
        return true;
    }

    public async void Subscribe(string topicName, SubscriptionDelegate subscriptionDelegate)
    {
        await mqttClient.SubscribeAsync(new TopicFilterBuilder().WithTopic(topicName).Build());

        mqttClient.ApplicationMessageReceived += async (s, e) => {
            if (topicName.Equals(e.ApplicationMessage.Topic))
            {
                var decryptedMessage = await riksKit.Decrypt(e.ApplicationMessage.Payload);
                subscriptionDelegate(System.Text.Encoding.UTF8.GetString(decryptedMessage.SecretData));
            }
        };
    }

    public async void Publish(string topicName, string contents)
    {
        var encryptedMessage = await riksKit.Encrypt(new Message(System.Text.Encoding.UTF8.GetBytes(contents)), topicName);

        var mqttMessage = new MqttApplicationMessageBuilder()
            .WithTopic(topicName)
            .WithPayload(encryptedMessage)
            .Build();
        await mqttClient.PublishAsync(mqttMessage);
    }

}
