#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Replace with your ESP-Server's actual MAC Address
uint8_t serverAddress[] = {0x32, 0xAE, 0xDF, 0x12, 0x34, 0x56};

const uint8_t AUTH_TOKEN[6] = {0x46, 0x52, 0x53, 0x48, 0x30, 0x31};

struct __attribute__((packed)) PacketData {
    uint8_t token[6];
    char payload[300];
};


void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    String response = "";
    for (int i = 0; i < len; i++) {
        response += (char)incomingData[i];
    }
    Serial.print("[Server Response]: ");
    Serial.println(response);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, serverAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
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
            memcpy(packet.token, AUTH_TOKEN, 6); 

            int payloadLen = hexInput.length() / 2;
            if (payloadLen >= sizeof(packet.payload)) payloadLen = sizeof(packet.payload) - 1;
            
            uint8_t tempPayload[300];
            hexToBytes(hexInput, tempPayload, payloadLen);
            
            memset(packet.payload, 0, sizeof(packet.payload));
            memcpy(packet.payload, tempPayload, payloadLen);

            esp_now_send(serverAddress, (uint8_t *)&packet, sizeof(packet));
        }
    }
}