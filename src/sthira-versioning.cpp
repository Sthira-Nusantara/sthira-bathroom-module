#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

WiFiClientSecure deviceClient;

const String FirmwareVer = {"5.0"};
#define URL_fw_Version "https://raw.githubusercontent.com/romheraldi/sthira-rfid-module/master/version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/romheraldi/sthira-rfid-module/master/firmware.bin"

bool version_check()
{
    Serial.println("Check latest firmware version");

    if ((WiFi.status() == WL_CONNECTED))
    {
        deviceClient.setInsecure();
        HTTPClient https;
        Serial.print("[HTTPS] begin...\n");
        if (https.begin(deviceClient, URL_fw_Version))
        {
            Serial.print("[HTTPS] GET...\n");
            int httpCode = https.GET();

            if (httpCode > 0)
            {
                Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
                {
                    String payload = https.getString();
                    payload.trim();
                    Serial.print("Latest Version: ");
                    Serial.println(payload);
                    if (payload.equals(FirmwareVer))
                    {
                        Serial.println("Device already on latest firmware version");
                    }
                    else
                    {
                        Serial.println("New firmware detected");
                        return true;
                    }
                }
            }
            else
            {
                Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                return false;
            }

            https.end();
            return false;
        }
        else
        {
            Serial.printf("[HTTPS] Unable to connect\n");
            return false;
        }
    }
}

void version_update()
{
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    Serial.println("Device ready to update");
    t_httpUpdate_return ret = ESPhttpUpdate.update(deviceClient, URL_fw_Bin);

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

    case HTTP_UPDATE_OK:
        Serial.println("Update Done well");
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
}