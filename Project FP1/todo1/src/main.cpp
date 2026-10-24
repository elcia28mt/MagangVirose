#include <Arduino.h>
#include <ArduinoJson.h>

// This C++ program runs on your Laptop (or via Serial Monitor input)
// It formats your order into JSON, converts it to HEX, and sends it out via Serial.

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== KASIR LAPTOP FRESHMART (C++) ===");
    Serial.println("Ketik perintah JSON di Serial Monitor, contoh:");
    Serial.println("{\"items\":[{\"id\":\"P002\",\"jumlah\":1}]}");
}

void loop() {
    // Check if you typed something in the Serial Monitor
    if (Serial.available() > 0) {
        String inputJson = Serial.readStringUntil('\n');
        inputJson.trim();

        if (inputJson.length() > 0) {
            // 1. Show original JSON
            Serial.print("\n[JSON Input]: ");
            Serial.println(inputJson);

            // 2. Convert JSON string to Hexadecimal representation (as required by rules)
            String hexString = "";
            for (size_t i = 0; i < inputJson.length(); i++) {
                char buf[3];
                sprintf(buf, "%02X", inputJson[i]);
                hexString += buf;
            }

            // 3. Send Hex string via Serial to ESP-Bridge
            Serial.print("[Send Hex to Bridge]: ");
            Serial.println(hexString);
        }
    }
}