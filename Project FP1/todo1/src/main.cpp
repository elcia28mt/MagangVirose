#include <Arduino.h>
#include <ArduinoJson.h>

void printHelp() {
    Serial.println("\n=======================================================================");
    Serial.println("       DAFTAR OPERASI KASIR FRESHMART     ");
    Serial.println("=======================================================================");
    Serial.println("1. Help                   - Daftar Bantuan");
    Serial.println("2. Menu                   - Daftar Produk");
    Serial.println("3. Order <ID> <jml>       - Membuat Pesanan (Contoh: order P002 1)");
    Serial.println("4. Status                 - Mengecek status");
    Serial.println("=======================================================================\n");
}

void printMenu() {
    Serial.println("\n--- KATALOG PRODUK FRESHMART ---");
    Serial.println("P001 | Beras 5kg      | Rp 65.000 | Stok: 20 | Maks: 2");
    Serial.println("P002 | Minyak Goreng  | Rp 32.000 | Stok: 15 | Maks: 1");
    Serial.println("P003 | Gula Pasir 1kg | Rp 15.500 | Stok: 30 | Maks: 5");
    Serial.println("P004 | Telur Ayam 1kg | Rp 28.000 | Stok: 25 | Maks: 3");
    Serial.println("---------------------------------\n");
}


void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\nSistem Kasir Laptop FreshMart Berhasil Dimulai.");
    printHelp();
}


void sendHex(const String &jsonStr) {
    String hexString = "";
    for (size_t i = 0; i < jsonStr.length(); i++) {
        char buf[3];
        sprintf(buf, "%02X", jsonStr[i]);
        hexString += buf;
    }
    Serial.println(hexString); // send ke esp bridge
}

void loop() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (input.equalsIgnoreCase("help") || input.equalsIgnoreCase("daftar operasi")) {
            printHelp();
        } 
        else if (input.equalsIgnoreCase("menu")) {
            printMenu();
        } 
        else if (input.equalsIgnoreCase("status")) {
            Serial.println("[STATUS]: Koneksi Serial Laptop ddengan ESP-Bridge Aktif (Baudrate: 115200).");
        } 
        else if (input.startsWith("order ")) {
            int firstSpace = input.indexOf(' ');
            int secondSpace = input.indexOf(' ', firstSpace + 1);
            if (secondSpace != -1) {
                String id = input.substring(firstSpace + 1, secondSpace);
                int jml = input.substring(secondSpace + 1).toInt();

                StaticJsonDocument<200> doc;
                JsonArray items = doc.createNestedArray("items");
                JsonObject item = items.createNestedObject();
                item["id"] = id;
                item["jumlah"] = jml;

                String jsonString;
                serializeJson(doc, jsonString);

                Serial.print("[JSON Terbentuk]: ");
                Serial.println(jsonString);
                Serial.print("[Send Hex to Bridge]: ");
                sendHex(jsonString);
            } else {
                Serial.println("[ERROR]: Format salah! Gunakan format: order <ID> <jumlah> (Contoh: order P002 1)");
            }
        } 
        else if (input.length() > 0) {
            Serial.print("[JSON Input]: ");
            Serial.println(input);
            Serial.print("[Send Hex to Bridge]: ");
            sendHex(input);
        }
    }
}