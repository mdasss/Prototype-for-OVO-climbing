//
// Created by matda on 04.06.2025.
//

#include "Ble_sender.hpp"

int Ble_sender::instance_counter = 0;

Ble_sender::Ble_sender(std::string id){
    device_number = ++instance_counter;
    device_ID = id;
    BLEDevice::init("BLE Sender");
    advertiser = BLEDevice::getAdvertising();
    configure_advertisement();
}

void Ble_sender::start_advertising(){
    advertiser -> start();
}

void Ble_sender::stop_advertising(){
    advertiser -> stop();
}

void Ble_sender::configure_advertisement() {

    adv_Data = BLEAdvertisementData();
    //adv_Data.setName("S nr." + std::to_string(device_number) + " ID:" + device_ID); // Sender name => Sender nr.1 ID:AA24124
    adv_Data.setName(device_ID);
    adv_Data.addData(std::string("\xff") + "ID:" + device_ID + ", S nr." + std::to_string(device_number)); // advertises [0xFF] + "ID:AA24124", manufacturer data

    advertiser->setAdvertisementData(adv_Data);
    advertiser->setScanResponse(false); // might be necessary to change
    advertiser->setMinInterval(1000); // 6250 ms
    advertiser->setMaxInterval(1000);

}

void Ble_sender::new_advertising_time(int time) {
    advertiser->setMinInterval(time);
    advertiser->setMaxInterval(time);
}