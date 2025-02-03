#include <WiFiManager.h>   
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <Update.h>

#define LED_BUILTIN 2  // Onboard LED for ESP32
const char* firmware_url = "http://your-server.com/firmware_v1.1.bin";  // New firmware URL

void updateFirmware() {
    WiFiClient client;
    HTTPClient http;

    Serial.print("Checking for firmware update from: ");
    Serial.println(firmware_url);
    
    http.begin(client, firmware_url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        int contentLength = http.getSize();
        if (contentLength <= 0) {
            Serial.println("Invalid content length, aborting...");
            return;
        }

        bool canBegin = Update.begin(contentLength);
        if (!canBegin) {
            Serial.println("Not enough space to begin OTA");
            return;
        }

        WiFiClient* stream = http.getStreamPtr();
        size_t written = Update.writeStream(*stream);

        if (written == contentLength) {
            Serial.println("Firmware update complete.");
            if (Update.end()) {
                Serial.println("Update successful, rebooting...");
                ESP.restart();
            } else {
                Serial.println("Update failed.");
            }
        } else {
            Serial.println("Update failed, written bytes mismatch.");
        }
    } else {
        Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}

void setup() {
    Serial.begin(115200);

    WiFiManager wifiManager;
    wifiManager.autoConnect("ESP32_AP");  // AP when no WiFi is found

    Serial.println("Connected to WiFi!");
    
    updateFirmware();  // Check for new firmware update

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    Serial.println("ESP32 Updated - Blinking LED");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
}
