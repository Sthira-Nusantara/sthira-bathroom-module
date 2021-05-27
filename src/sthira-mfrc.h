#ifndef STHIRAMFRC_H
#define STHIRAMFRC_H

void mfrc_init();
bool mfrc_isNewCardPreset();
bool mfrc_readCardSerial();
const char *mfrc_contentPrint();

#endif