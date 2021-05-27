#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>

#include "sthira-opening.h"
#include "sthira-wifi.h"
#include "sthira-mfrc.h"

void setup()
{
  Serial.begin(115200);

  printOpening();

  SPI.begin();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  delay(100);
  // Init MFRC522
  mfrc_init();

  delay(100);
  setup_wifi();

  Serial.println("Setup done");
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    setup_wifi();
  }

  if (!mfrc_isNewCardPreset())
  {
    return;
  }

  if (!mfrc_readCardSerial())
  {
    return;
  }

  Serial.println(mfrc_contentPrint());

  delay(1000);
}