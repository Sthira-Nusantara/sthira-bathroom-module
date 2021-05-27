#ifndef STHIRAMFRC_H
#define STHIRAMFRC_H
#include <MFRC522.h>

extern MFRC522 mfrc522;

void mfrc_init();
bool mfrc_isNewCardPreset();
bool mfrc_readCardSerial();
const char *mfrc_contentPrint();

#endif