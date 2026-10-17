#include<main.h>

esp_now_peer_info_t peer_info;

uint8_t mac_addresses[MAC_ADDRESS_TOTAL][MAC_ADDRESS_LENGTH] = {
    {0x24, 0x0A, 0xC4, 0x0A, 0x21, 0x11},  // FARUQ
    {0x24, 0x0A, 0xC4, 0x0A, 0x21, 0x10},  // FAUZAN
    {0x24, 0x0A, 0xC4, 0x0A, 0x20, 0x11},  // RAFAINA
    {0x24, 0x0A, 0xC4, 0x0A, 0x10, 0x10},  // Danish Fattah Azizi
    {0x24, 0x0A, 0xC4, 0x0A, 0x10, 0x11},  // Marsya Safira
    {0x24, 0x0A, 0xC4, 0x0A, 0x11, 0x10},  // Faqih Al Minanji
    {0x24, 0x0A, 0xC4, 0x0A, 0x11, 0x11},  // Farid Azzam Arrayyan
    {0x24, 0x0A, 0xC4, 0x0A, 0x12, 0x10},  // Kenzi Attabany
    {0x24, 0x0A, 0xC4, 0x0A, 0x12, 0x11},  // Nabila Fathania Rizqi Romadlon
    {0x24, 0x0A, 0xC4, 0x0A, 0x13, 0x10},  // Felicia Metta Tania
};

const char *mac_names[MAC_ADDRESS_TOTAL] = {
    "FARUQ",                               // 0
    "FAUZAN",                              // 1
    "RAFAINA",                             // 2
    "Danish Fattah Azizi",                 // 3
    "Marsya Safira",                       // 4
    "Faqih Al Minanji",                    // 5
    "Farid Azzam Arrayyan",                // 6
    "Kenzi Attabany",                      // 7
    "Nabila Fathania Rizqi Romadlon",      // 8
    "Felicia Metta Tania",                 // 9
};

esp_err_t mulai_esp_now(int index_mac_address) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    /* Init ESP-NOW */
    esp_err_t result = esp_now_init();
    if (result != ESP_OK)
        return result;

    /* Set callback function to handle received data */
    result = esp_now_register_recv_cb(callback_data_esp_now);
    if (result != ESP_OK)
        return result;

    result = esp_now_register_send_cb(callback_pengiriman_esp_now);
    //     if (result != ESP_OK)
    //         return result;

    /* Set MAC Address */
    uint8_t mac[MAC_ADDRESS_LENGTH];
    memcpy(mac, mac_addresses[index_mac_address], MAC_ADDRESS_LENGTH);
    result = esp_wifi_set_mac(WIFI_IF_STA, mac);
    if (result != ESP_OK)
        return result;

    /* Initialize peer_info and set fields*/
    memset(&peer_info, 0, sizeof(esp_now_peer_info_t));
    peer_info.channel = 0;
    peer_info.encrypt = false;

    /* Add All MAC to peer list  */
    for (int i = 0; i < MAC_ADDRESS_TOTAL; i++) {
        memcpy(peer_info.peer_addr, mac_addresses[i], MAC_ADDRESS_LENGTH);
        result = esp_now_add_peer(&peer_info);
        if (result != ESP_OK)
            return result;
    }

    return ESP_OK;
}

int cari_mac_index(const uint8_t *mac) {
    for (int i = 0; i < MAC_ADDRESS_TOTAL; i++) {
        // Compare the MAC address
        if (memcmp(mac, mac_addresses[i], MAC_ADDRESS_LENGTH) == 0)
            return i;
    }

    // if not found return -1
    return -1;
}

String mac_index_to_names(int mac_index) {
    if ((mac_index < 0 || mac_index >= MAC_ADDRESS_TOTAL) || (mac_index == -1)) {
        return "Unknown";
    }
    return mac_names[mac_index];
}

void callback_data_esp_now(const uint8_t *mac, const uint8_t *data, int len) {
    int index_mac_asal = cari_mac_index(mac);
    process_perintah(data, len, index_mac_asal);
}
void callback_pengiriman_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.printf("\nStatus pengiriman ESP-NOW: %s\n", esp_err_to_name(status));
}
void callback_data_serial(const uint8_t *data, int len) {
    process_perintah(data, len);
}
void baca_serial(void (*callback)(const uint8_t *data, int len)) {
    // TODO 2: implementasi kode buat nerima perintah dari serial
}

void process_perintah(const uint8_t *data, int len, int index_mac_address_asal) {
    // TODO 3: implementasi kode buat processing perintah
}