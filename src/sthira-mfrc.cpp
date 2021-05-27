#include "sthira-mfrc.h"
#include <Arduino.h>
#include <MFRC522.h>

#define SS_PIN 4
#define RST_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN);

void mfrc_init()
{
    mfrc522.PCD_Init();

    mfrc522.PCD_DumpVersionToSerial();
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

bool mfrc_isNewCardPreset()
{
    return mfrc522.PICC_IsNewCardPresent();
}

bool mfrc_readCardSerial()
{
    return mfrc522.PICC_ReadCardSerial();
}

const char *mfrc_contentPrint()
{

    Serial.println();
    Serial.print(" UID tag :");
    String content = "";

    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    content.toUpperCase();
    Serial.print(content);

    return content.c_str();
}