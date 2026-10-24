#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>

const uint8_t VALID_TOKEN[6] = {0x46, 0x52, 0x53, 0x48, 0x30, 0x31};

struct Produk {
    String id;
    String nama;
    double harga;
    int stok;
    int batasMaks;
};

// Simulasi Database Katalog Produk[cite: 1]
Produk katalog[] = {
    {"P001", "Beras 5kg", 65000, 20, 2},
    {"P002", "Minyak Goreng 2L", 32000, 15, 1},
    {"P003", "Gula Pasir 1kg", 15500, 30, 5},
    {"P004", "Telur Ayam 1kg", 28000, 25, 3}
};
const int jumlahProduk = 4;

struct __attribute__((packed)) PacketData {
    uint8_t token[6];
    char payload[200]; // Berisi string JSON dari laptop
};

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    PacketData packet;
    if (len < sizeof(packet)) {
        Serial.println("Error: Ukuran paket tidak valid!");
        return;
    }
    memcpy(&packet, incomingData, sizeof(packet));

    for (int i = 0; i < 6; i++) {
        if (packet.token[i] != VALID_TOKEN[i]) {
            Serial.println("Error: Token autentikasi ditolak!");
            return;
        }
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, packet.payload);
    if (error) {
        Serial.println("Error: Format JSON tidak valid!");
        return;
    }

    String idProduk = doc["id"];
    int jumlahBeli = doc["jumlah"];

    bool produkDitemukan = false;
    double totalBelanja = 0;
    String pesanRespon = "";

    for (int i = 0; i < jumlahProduk; i++) {
        if (katalog[i].id == idProduk) {
            produkDitemukan = true;

            if (jumlahBeli <= 0) {
                pesanRespon = "Gagal: Jumlah beli harus angka positif!";
            } else if (jumlahBeli > katalog[i].batasMaks) {
                pesanRespon = "Gagal: Melebihi batas pembelian maksimum!";
            } else if (jumlahBeli > katalog[i].stok) {
                pesanRespon = "Gagal: Stok produk tidak mencukupi!";
            } else if (katalog[i].stok == 0) {
                pesanRespon = "Gagal: Produk dengan stok 0 tidak dapat dijual!";
            } else {
                katalog[i].stok -= jumlahBeli;
                totalBelanja = katalog[i].harga * jumlahBeli;
                pesanRespon = "Sukses! NoStruk: STR-" + String(millis()) + ", Item: " + katalog[i].nama + ", Total: Rp " + String(totalBelanja);
            }
            break;
        }
    }

    if (!produkDitemukan) {
        pesanRespon = "Gagal: ID produk tidak ditemukan di katalog!";
    }

    esp_now_send(mac_addr, (uint8_t *)pesanRespon.c_str(), pesanRespon.length() + 1);
}

void setup() {
    Serial.begin(115200);
    
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Gagal menginisialisasi ESP-NOW!");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
    
    Serial.println("ESP-Server FreshMart Siap Beroperasi.");
}

void loop() {
  
}