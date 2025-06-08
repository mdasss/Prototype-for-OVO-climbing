//
// Created by matda on 04.06.2025.
//

#include "Ble_reciver.hpp"
#include <Arduino.h>

int Ble_reciver::instance_counter = 0;

Ble_reciver::Ble_reciver(const std::string& receiver_ID, const std::array<std::string,2>& sender_prefixes, uint32_t scan_seconds, int rssi_threshold)
    :receiver_ID(receiver_ID),
    sender_prefixes(sender_prefixes),
    pBLEScan(nullptr),
    scan_seconds(scan_seconds),
    rssi_threshold(rssi_threshold)
{
    BLEDevice::init("");
    device_number = ++instance_counter;

    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true); // might be necessary to change to some other func
    pBLEScan->setInterval(3000); // each 3s the scan will occur
    pBLEScan->setWindow(1000); // time the radio is actually on
}

Ble_reciver::~Ble_reciver(){
    stop_scanning();
    if (pBLEScan){
        delete pBLEScan;
        pBLEScan = nullptr;
    }
}

std::string Ble_reciver::get_manufacturer_payload(const std::string &raw_data) const {
    if (raw_data.empty() || static_cast<uint8_t>(raw_data[0]) != 0xFF){
        return "";
    }
    return raw_data.substr(1); // returns ASCII-part, should be "ID::AA24124, S nr.1"
}

bool Ble_reciver::is_valid_sender_id(const std::string &id) const {
    for (const auto& prefix : sender_prefixes){
        if (id.rfind(prefix,0) == 0){
            for (size_t i = prefix.size(); i <id.size(); i++){
                if (!isDigit(id[i])){
                    return false;
                }
            }
            return true;
            }
        }
    return false;
}

void Ble_reciver::scann_and_process() {
    if (!pBLEScan){
        Serial.println("pBLEScan is not initialized!"); // println does not take std::string as an argument, but rather a c-style string
        return;
    }
    BLEScanResults found_senders = pBLEScan->start(scan_seconds,false);

    int best_RSSI = rssi_threshold;   // the strongest signal
    std::string  best_id;
    std::vector<uint8_t> best_raw_data;
    bool found = false;

    int count = found_senders.getCount();
    for (int i = 0; i < count; i++){
        BLEAdvertisedDevice adv_data = found_senders.getDevice(i);

        if (!adv_data.haveName()){
            continue;
        }

        std::string sender_ID = adv_data.getName();

        if (!is_valid_sender_id((sender_ID))){
            continue;
        }

        int rssi = adv_data.getRSSI();
        if (rssi <= rssi_threshold){
            continue;
        }

        if (!adv_data.haveManufacturerData()){
            continue;
        }
        std::string raw_data = adv_data.getManufacturerData();

        if (rssi > best_RSSI || !found){
            best_RSSI = rssi;
            best_id = sender_ID;
            best_raw_data.assign(reinterpret_cast<const uint8_t*>(raw_data.data()), reinterpret_cast<const uint8_t*>(raw_data.data())+ raw_data.length());
            found = true;
        }

        pBLEScan -> clearResults();

        if (found){
            Serial.printf("The nearest valid sender device with ID=%s | RSSI=%d dBm\n",best_id.c_str(),best_RSSI);
            handle_incoming_data(best_id,best_raw_data.data(),best_raw_data.size());
        }else{
            Serial.println("None valid pointer (over -64 dBm) found this iteration.");
        }
    }
}

void  Ble_reciver::stop_scanning() {
    if (pBLEScan){
        pBLEScan->stop();

    }
}

void Ble_reciver::handle_incoming_data(const std::string &device_ID, const uint8_t *raw_data, size_t length) {
    Serial.printf("Treating the data from [ID=%s] | Length=%u\n",device_ID.c_str(),(unsigned)length);

    for (size_t i = 0; i< length; ++i){
        Serial.printf("%02X", raw_data[i]);
    }
    Serial.println();

    std::string payload(reinterpret_cast<const char*>(raw_data),length);
    std::string ascii = get_manufacturer_payload(payload); // ascii = "ID:AA0001 ,S nr.1"

    size_t pos = ascii.find(",S nr.");
    if (pos != std::string::npos){
        std::string sn = ascii.substr(pos+6);
        Serial.printf(" Sender-number: %s\n",sn.c_str());
    }
}
