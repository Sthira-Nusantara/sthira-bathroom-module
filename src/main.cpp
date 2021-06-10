#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <FastLED.h>

#include "sthira-opening.h"
#include "sthira-versioning.h"
#include "sthira-wifi.h"
#include "sthira-mfrc.h"
#include "sthira-pubsub.h"

#define NUM_LEDS 1
#define DATA_PIN 3

CRGB leds[NUM_LEDS];

int locked = 0;
int updateLocked = 0;

void changeColor(CRGB color)
{
  for (int i = 0; i <= NUM_LEDS; i++)
  {
    leds[i] = color;
  }
  FastLED.show();
}

void setup()
{
  Serial.begin(115200);

  printOpening();

  SPI.begin();
  Serial.println();

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  pinMode(A0, INPUT);

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

    if(analogRead(A0) > 1000) {
      locked = 1;
    } else {
      locked: 0;
    }

    if(locked == LOW) {
      digitalWrite(2, LOW);

      changeColor(CRGB::Red);

      updateLocked = locked;
    } else if (locked == HIGH && updateLocked == LOW) {
      changeColor(CRGB::Blue);

      delay(5000);

      digitalWrite(2, HIGH);
      updateLocked = locked;
    }

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