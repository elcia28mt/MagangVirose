#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 128
#define ADDR_TEST 10

unsigned long previousMillis = 0;
bool ledState = false;

void setup() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);

    // Initialize LED pin
    pinMode(LED_BUILTIN, OUTPUT);

    // Uncomment salah satu baris di bawah untuk mengatur nilai EEPROM
    // EEPROM.write(ADDR_TEST, 0);  // LED mati
    // EEPROM.write(ADDR_TEST, 1);  // Berkedip 300ms
    // EEPROM.write(ADDR_TEST, 2);  // Berkedip 1000ms
    // EEPROM.write(ADDR_TEST, 3);  // Menyala terus
    // EEPROM.commit();  // Jangan lupa commit setelah write

    // Read current value from EEPROM
    int mode = EEPROM.read(ADDR_TEST);
    Serial.printf("LED Mode from EEPROM: %d\n", mode);

    switch(mode) {
        case 0:
            Serial.println("Mode 0: LED OFF");
            break;
        case 1:
            Serial.println("Mode 1: LED Blink 300ms");
            break;
        case 2:
            Serial.println("Mode 2: LED Blink 1000ms");
            break;
        case 3:
            Serial.println("Mode 3: LED Always ON");
            break;
        default:
            Serial.println("Unknown mode, LED OFF");
            break;
    }
}

void loop() {
    // Read LED mode from EEPROM
    int ledMode = EEPROM.read(ADDR_TEST);
    unsigned long currentMillis = millis();

    switch(ledMode) {
        case 0:
            // LED mati
            digitalWrite(LED_BUILTIN, LOW);
            break;

        case 1:
            // Berkedip setiap 300ms
            if (currentMillis - previousMillis >= 300) {
                previousMillis = currentMillis;
                ledState = !ledState;
                digitalWrite(LED_BUILTIN, ledState);
            }
            break;

        case 2:
            // Berkedip setiap 1000ms
            if (currentMillis - previousMillis >= 1000) {
                previousMillis = currentMillis;
                ledState = !ledState;
                digitalWrite(LED_BUILTIN, ledState);
            }
            break;

        case 3:
            // Menyala terus
            digitalWrite(LED_BUILTIN, HIGH);
            break;

        default:
            // Default: LED mati jika nilai tidak dikenali
            digitalWrite(LED_BUILTIN, LOW);
            break;
    }

    // Optional: Check for serial input to change mode
     if (Serial.available()) {
        int newMode = Serial.read();
        if (newMode >= 0 && newMode <= 3) {
            EEPROM.write(ADDR_TEST, newMode);
            EEPROM.commit();
            Serial.printf("LED mode changed to: %d\n", newMode);
        }
    }
}