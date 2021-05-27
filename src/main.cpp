#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>

#include "sthira-opening.h"
#include "sthira-versioning.h"
#include "sthira-wifi.h"
#include "sthira-mfrc.h"
#include "sthira-pubsub.h"

void setup()
{
  Serial.begin(115200);

  printOpening();

  SPI.begin();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  delay(100);
  mfrc_init();

  delay(100);
  setup_wifi();

  if (version_check())
  {
    version_update();
  }

  pubsub_init();

  Serial.println("Setup done");
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    setup_wifi();
  }

  if (!pubsub_connected())
  {
    Serial.println("MQTT is NOT connected");
    int lastReconnectAttempt = 0;
    long now = millis();
    if (now - lastReconnectAttempt > 5000)
    {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (pubsub_connected())
      {
        lastReconnectAttempt = 0;
      }
    }
  }
  else
  {
    client.loop();

    if (!mfrc_isNewCardPreset())
    {
      return;
    }

    if (!mfrc_readCardSerial())
    {
      return;
    }

    Serial.println();
    Serial.print(" UID tag :");
    String content = "";

    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    content.toUpperCase();
    Serial.print(content);

    char charBuf[content.length() + 1];
    content.toCharArray(charBuf, content.length() + 1);

    if (content != "")
    {
      client.publish(getCard.c_str(), charBuf);
    }

    delay(1000);
  }
}