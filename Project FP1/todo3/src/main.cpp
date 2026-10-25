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

Produk katalog[] = {
    {"P001", "Beras 5kg", 65000, 20, 2},
    {"P002", "Minyak Goreng 2L", 32000, 15, 1},
    {"P003", "Gula Pasir 1kg", 15500, 30, 5},
    {"P004", "Telur Ayam 1kg", 28000, 25, 3}
};
const int jumlahProduk = 4;

struct __attribute__((packed)) PacketData {
    uint8_t header[4];
    uint8_t source;
    uint8_t destination;
    uint8_t token[6];
    uint16_t length;
    char payload[230];
};

String decodeHexPayload(const char* hexBytes, uint16_t dataLen) {
    String decodedJson = "";
    for (uint16_t i = 0; i < dataLen; i += 2) {
        char byteStr[3] = { hexBytes[i], hexBytes[i+1], '\0' };
        char asciiChar = (char)strtol(byteStr, NULL, 16);
        decodedJson += asciiChar;
    }
    return decodedJson;
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    if (len < sizeof(PacketData)) return;
    PacketData packet;
    memcpy(&packet, incomingData, sizeof(PacketData));

    if (packet.header[0] != 'F' || packet.header[1] != 'M' || packet.header[2] != 'R' || packet.header[3] != 'T') {
        String errCode = "E02";
        esp_now_send(mac_addr, (uint8_t *)errCode.c_str(), errCode.length() + 1);
        return; 
    }

    for (int i = 0; i < 6; i++) {
        if (packet.token[i] != VALID_TOKEN[i]) {
            String errCode = "E01";
            esp_now_send(mac_addr, (uint8_t *)errCode.c_str(), errCode.length() + 1);
            return;
        }
    }

    if (packet.length == 0 || packet.length > 230) {
        String errCode = "E03";
        esp_now_send(mac_addr, (uint8_t *)errCode.c_str(), errCode.length() + 1);
        return;
    }

    String jsonString = decodeHexPayload(packet.payload, packet.length);

    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    if (error) {
        String errCode = "E05";
        esp_now_send(mac_addr, (uint8_t *)errCode.c_str(), errCode.length() + 1);
        return;
    }

    JsonArray items = doc["items"];
    double totalBelanja = 0;
    String pesanRespon = "";
    bool transaksiBerhasil = true;
    int tempStokChanges[jumlahProduk] = {0};

    for (JsonObject item : items) {
        String idProduk = item["id"];
        int jumlahBeli = item["jumlah"];

        bool produkDitemukan = false;
        for (int i = 0; i < jumlahProduk; i++) {
            if (katalog[i].id == idProduk) {
                produkDitemukan = true;

                if (jumlahBeli <= 0 || jumlahBeli > katalog[i].batasMaks || jumlahBeli > katalog[i].stok || katalog[i].stok == 0) {
                    pesanRespon = "E09"; // E09: Proses bisnis gagal (aturan stok / batas beli)[cite: 7]
                    transaksiBerhasil = false;
                } else {
                    totalBelanja += katalog[i].harga * jumlahBeli;
                    tempStokChanges[i] += jumlahBeli;
                }
                break;
            }
        }

        if (!produkDitemukan) {
            pesanRespon = "E08";
            transaksiBerhasil = false;
            break;
        }
        if (!transaksiBerhasil) break;
    }

    if (transaksiBerhasil) {
        for (int i = 0; i < jumlahProduk; i++) {
            katalog[i].stok -= tempStokChanges[i];
        }
        pesanRespon = "Sukses! NoStruk: STR-" + String(millis()) + ", Total: Rp " + String(totalBelanja);
    }

    esp_now_send(mac_addr, (uint8_t *)pesanRespon.c_str(), pesanRespon.length() + 1);
    Serial.println(pesanRespon);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) return;
    esp_now_register_recv_cb(OnDataRecv);
    Serial.println("ESP-Server FreshMart Siap.");
}

void loop() {}