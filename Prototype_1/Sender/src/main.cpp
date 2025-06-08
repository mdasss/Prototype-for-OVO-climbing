#include <Arduino.h>
#include "Ble_sender.hpp"


Ble_sender* sender1;
Ble_sender* sender2;


void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("Sender started!");
    sender1 = new Ble_sender("AA0001");
    //sender2 = new Ble_sender("AA0002");
    sender1->start_advertising();
}

void loop() {
    delay(1000);
    Serial.println("running");
}
