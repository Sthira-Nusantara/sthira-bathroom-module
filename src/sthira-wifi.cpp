#include "sthira-wifi.h"
#include <ESP8266WiFi.h>
#include <Arduino.h>

const char *ssid = "IOT_STHIRA";
const char *password = "IotSthiraNusantara@2712";

void setup_wifi()
{
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    int tryingToConnect = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        if (tryingToConnect < 20)
        {
            delay(250);
            Serial.print(".");
            delay(250);
            tryingToConnect = tryingToConnect + 1;
        }
        else
        {
            ESP.restart();
        }
    }

    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}