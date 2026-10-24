#include <Arduino.h>
#include <ArduinoJson.h>

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\nKASIR LAPTOP FRESHMART");
    Serial.println("Ketik perintah JSON di Serial Monitor, contoh:");
    Serial.println("{\"items\":[{\"id\":\"P002\",\"jumlah\":1}]}");
}

void loop() {
    if (Serial.available() > 0) {
        String inputJson = Serial.readStringUntil('\n');
        inputJson.trim();

        if (inputJson.length() > 0) {
            Serial.print("\n[JSON Input]: ");
            Serial.println(inputJson);
            String hexString = "";
            for (size_t i = 0; i < inputJson.length(); i++) {
                char buf[3];
                sprintf(buf, "%02X", inputJson[i]);
                hexString += buf;
            }
            
            Serial.print("[Send Hex to Bridge]: ");
            Serial.println(hexString);
        }
    }
}