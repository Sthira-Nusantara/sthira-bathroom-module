#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "sthira-versioning.h"

const char *mqtt_server = "mqtt.rupira.com";
const char *mqtt_username = "sthirasystemiot";
const char *mqtt_password = "Ud3^fn*mpWWKaLw8oRUBG5FY&NQ2A@cK^iVW*7FZCRqCgnKYEQ6wBHh5K%J9uwCBW%KGBrg$Kv5Bgx$yCBGR3UuhSZq%89XFHf@6^cJjRamB7FntyAG#78%wdYhN!YYX";

String MacAdd = String(WiFi.macAddress());

String COMPANY = "sthira";
String DEVICE = "controller";
String UNUM = MacAdd;

String prefix = COMPANY + "/" + DEVICE + "/" + UNUM;
// ----- Subscribe Init
String getCard = prefix + "/getCard";
String cardOk = prefix + "/cardOk";
String cardFailed = prefix + "/cardFailed";
String update = prefix + "/update";
String restart = prefix + "/restart";
String testSubs = prefix + "/test";
String checkConnection = prefix + "/check";
String yesConnect = prefix + "/yesConnect";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    if (strcmp(topic, update.c_str()) == 0)
    {
        Serial.println("Checking firmware update");
        if (version_check())
        {
            version_update();
        }
    }

    if (strcmp(topic, restart.c_str()) == 0)
    {
        ESP.restart();
    }

    if (strcmp(topic, checkConnection.c_str()) == 0)
    {
        // client.publish(yesConnect.c_str(), "1");
    }

    Serial.println();
}

void pubsub_init()
{
    Serial.print("Mac Address : ");
    Serial.println(MacAdd);
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

bool pubsub_connected()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str(), mqtt_username, mqtt_password))
        {
            Serial.println("connected");
            client.publish("connected", MacAdd.c_str());

            // Your Subs
            client.subscribe(restart.c_str());
            client.subscribe(update.c_str());
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }

    return client.connected();
}
