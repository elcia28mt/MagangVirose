#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

uint8_t serverAddress[] = {0xD0, 0xEF, 0x76, 0x32, 0x55, 0xB8}; // Ganti MAC Server
const uint8_t AUTH_TOKEN[6] = {0x46, 0x52, 0x53, 0x48, 0x30, 0x31};

struct __attribute__((packed)) PacketData {
    uint8_t header[4];
    uint8_t source;
    uint8_t destination;
    uint8_t token[6];
    uint16_t length;
    char payload[230];
};

// Pemetaan kode error resmi sesuai dokumentasi README
String translateResponse(const String &rawResponse) {
    if (rawResponse == "E01") return "Token autentikasi tidak sesuai.";
    if (rawResponse == "E02") return "Paket tidak memiliki header yang benar.";
    if (rawResponse == "E03") return "Panjang payload tidak sesuai dengan data aktual.";
    if (rawResponse == "E04") return "Payload berisi karakter atau format hexadecimal yang salah.";
    if (rawResponse == "E05") return "Payload tidak dapat diparsing sebagai JSON.";
    if (rawResponse == "E06") return "Jenis permintaan tidak didukung oleh server.";
    if (rawResponse == "E07") return "Parameter wajib tidak tersedia atau nilainya tidak sesuai.";
    if (rawResponse == "E08") return "Data yang diminta tidak tersedia.";
    if (rawResponse == "E09") return "Permintaan ditolak karena aturan proses bisnis.";
    if (rawResponse == "E10") return "Respons tidak diterima dalam batas waktu yang ditentukan.";
    if (rawResponse == "E11") return "Identitas tujuan paket tidak sesuai.";
    if (rawResponse == "E12") return "Terjadi kesalahan internal saat memproses permintaan.";
    
    return rawResponse; // Jika pesan sukses / struk, tampilkan langsung
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    String response = "";
    for (int i = 0; i < len; i++) {
        response += (char)incomingData[i];
    }
    response.trim();
    Serial.print("[RESP]: ");
    Serial.println(translateResponse(response));
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) return;
    esp_now_register_recv_cb(OnDataRecv);

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, serverAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) return;
    Serial.println("ESP-Bridge FreshMart Siap.");
}

void hexToBytes(const String &hexStr, uint8_t *byteArray, int byteLen) {
    for (int i = 0; i < byteLen; i++) {
        String byteString = hexStr.substring(i * 2, i * 2 + 2);
        byteArray[i] = (uint8_t)strtol(byteString.c_str(), NULL, 16);
    }
}

void loop() {
    if (Serial.available() > 0) {
        String hexInput = Serial.readStringUntil('\n');
        hexInput.trim();

        if (hexInput.length() > 0) {
            PacketData packet;
            packet.header[0] = 'F'; packet.header[1] = 'M'; packet.header[2] = 'R'; packet.header[3] = 'T';
            packet.source = 0x01;      
            packet.destination = 0x02; 
            memcpy(packet.token, AUTH_TOKEN, 6); 

            int payloadLen = hexInput.length() / 2;
            if (payloadLen >= sizeof(packet.payload)) return;
            packet.length = payloadLen;
            
            uint8_t tempPayload[300];
            hexToBytes(hexInput, tempPayload, payloadLen);
            
            memset(packet.payload, 0, sizeof(packet.payload));
            memcpy(packet.payload, tempPayload, payloadLen);

            esp_now_send(serverAddress, (uint8_t *)&packet, sizeof(packet));
        }
    }
}