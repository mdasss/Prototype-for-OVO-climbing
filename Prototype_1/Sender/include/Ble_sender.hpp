//
// Created by Mateusz Dabrowa on 04.06.2025.
//

#ifndef PROTOTYPE_1_BLE_SENDER_HPP
#define PROTOTYPE_1_BLE_SENDER_HPP

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>


class Ble_sender {
    public:
        explicit Ble_sender(std::string id);
        void start_advertising();
        void stop_advertising();
        std::string get_ID() const;


    private:
        std::string device_ID;
        BLEAdvertising* advertiser;
        BLEAdvertisementData adv_Data;
        int device_number;
        static int instance_counter;


        void configure_advertisement();
        void new_advertising_time(int time);
        void update_advertiser(std::string id, BLEAdvertisementData data );
};


#endif //PROTOTYPE_1_BLE_SENDER_HPP
