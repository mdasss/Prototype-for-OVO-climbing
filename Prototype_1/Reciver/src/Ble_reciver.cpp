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
    if (!pBLEScan) {
        Serial.println("pBLEScan not initialized!");
        return;
    }

    // 1) Start the blocking scan
    BLEScanResults results = pBLEScan->start(scan_seconds, false);

    // 2) Pick the best (strongest) valid sender in one pass
    int  bestRSSI = INT_MIN;
    std::string bestID;
    std::vector<uint8_t> bestRaw;
    bool found = false;

    int count = results.getCount();
    for (int i = 0; i < count; i++) {
        BLEAdvertisedDevice dev = results.getDevice(i);

        if (!dev.haveName()) {
            Serial.println("  • The device has no name");
            continue;
        }
        std::string name = dev.getName();
        if (!is_valid_sender_id(name)) {
            Serial.printf("  • The device \"%s\" has invalid prefix\n", name.c_str());
            continue;
        }
        int rssi = dev.getRSSI();
        if (rssi <= rssi_threshold) {
            Serial.printf("  • The device is \"%s\" too far (RSSI=%d)\n", name.c_str(), rssi);
            continue;
        }
        if (!dev.haveManufacturerData()) {
            Serial.printf("  • The device has \"%s\" no manufacturer data\n", name.c_str());
            continue;
        }

        // we’ve got a valid candidate — keep the strongest
        if (!found || rssi > bestRSSI) {
            found   = true;
            bestRSSI = rssi;
            bestID   = name;

            std::string raw = dev.getManufacturerData();
            bestRaw.assign(
                    reinterpret_cast<const uint8_t*>(raw.data()),
                    reinterpret_cast<const uint8_t*>(raw.data()) + raw.length()
            );
        }
    }

    // 3) Clear all results _once_
    pBLEScan->clearResults();

    // 4) Report exactly one summary line
    if (found) {
        Serial.printf("→ Nearest sender: ID=%s  RSSI=%d dBm\n",bestID.c_str(), bestRSSI);
        handle_incoming_data(bestID, bestRaw.data(), bestRaw.size());
    }
    else {
        Serial.println("→ No valid sender found this cycle.");
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

    Serial.printf("ASCII payload: %s\n", ascii.c_str());

    size_t pos = ascii.find(",S nr.");
    if (pos != std::string::npos){
        std::string sn = ascii.substr(pos+6);
        Serial.printf(" Sender-number: %s\n",sn.c_str());
    }
}
