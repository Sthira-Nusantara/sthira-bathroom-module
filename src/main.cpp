#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
// #include <FastLED.h>

#include "sthira-opening.h"
#include "sthira-versioning.h"
#include "sthira-wifi.h"
#include "sthira-mfrc.h"
#include "sthira-pubsub.h"

// #define NUM_LEDS 1
// #define DATA_PIN 3
// CRGB leds[NUM_LEDS];

int locked = 0;
int updateLocked = 0;

void setup()
{
  Serial.begin(115200);

  printOpening();

  SPI.begin();
  Serial.println();

  pinMode(A0, INPUT);
  pinMode(16, OUTPUT); // KUNCI
  pinMode(0, OUTPUT); //  LAMPU
  pinMode(2, OUTPUT); // LAMPU LUAR

  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  delay(100);
  mfrc_init();

  delay(250);

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
  client.loop();

  if (analogRead(A0) > 1000)
  {
    locked = 1;
  }
  else
  {
    locked = 0;
  }

  if (locked == LOW)
  {
    digitalWrite(2, LOW);
    digitalWrite(0, LOW);

    updateLocked = locked;
  }
  else if (locked == HIGH && updateLocked == LOW)
  {
    digitalWrite(2, HIGH);

    delay(30000);

    digitalWrite(0, HIGH);
    updateLocked = locked;
  }

  delay(500);

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
    digitalWrite(2, LOW);
    client.publish(getCard.c_str(), charBuf);
    delay(1000);
    digitalWrite(2, HIGH);
  }

  delay(500);
}