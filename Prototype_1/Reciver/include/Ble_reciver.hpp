//
// Created by matda on 04.06.2025.
//

#ifndef PROTOTYPE_1_BLE_RECIVER_HPP
#define PROTOTYPE_1_BLE_RECIVER_HPP

#include <string>
#include <array>
#include <vector>
#include <BLEDevice.h>
#include <BLEAdvertisedDevice.h>
#include <BLEScan.h>



class Ble_reciver {
public:
    static int instance_counter;

    Ble_reciver(const std::string& receiver_ID,
                const std::array<std::string,2>& sender_prefixes,
                uint32_t scan_seconds,
                int rssi_threshold);

    void scann_and_process();
    void stop_scanning();

    void handle_incoming_data(const std::string& device_ID, const uint8_t* raw_data, size_t length);
    bool is_valid_sender_id(const std::string& id) const;
    std::string get_manufacturer_payload(const std::string & raw_data) const;

    ~Ble_reciver();
private:
    std::string receiver_ID;
    std::array<std::string,2> sender_prefixes; // remember to change, if more cities will be taken into account
    BLEScan* pBLEScan;

    std::string sender_name;
    uint32_t scan_seconds;
    int rssi_threshold;
    int device_number;
    std::string lacking_pBLEScan;

};


#endif //PROTOTYPE_1_BLE_RECIVER_HPP
