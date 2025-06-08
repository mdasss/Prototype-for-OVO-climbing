#include <Arduino.h>
#include "Ble_reciver.hpp" // Change the name to be correct ;=

static const std::array<std::string, 2> SENDER_PREFIXES = {"AA0","MO0"}; // AA0 -> AAlesund center nr.1, AA1 -> AAlesund center nr.2, MO0 -> Molde center nr.1, AA1 -> Molde center nr.2,
static const uint32_t  SCAN_DURATION_SEC = 2;
static const int RSSI_THRESHOLD = -100;

Ble_reciver* ble_reciver = nullptr;

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== Starting a BLE-receiver ===");

    ble_reciver = new Ble_reciver(
            "Receiver1",
            SENDER_PREFIXES,
            SCAN_DURATION_SEC,
            RSSI_THRESHOLD
            );

    Serial.printf("Listening after sender devices with prefix \"%s\",\"%s\" each %u s, RSSI-threshold %d dBm\n\n",
                  SENDER_PREFIXES[0].c_str(),
                  SENDER_PREFIXES[1].c_str(),
                  SCAN_DURATION_SEC,
                  RSSI_THRESHOLD
                  );
}

void loop() {
    ble_reciver->scann_and_process();
    delay(500);
}