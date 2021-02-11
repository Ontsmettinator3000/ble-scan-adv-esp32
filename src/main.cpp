#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLECast.h>
#include <iostream>
#include <string>

#ifndef BEACON

int freq = 10;
int ledChannel = 0;
int resolution = 8;

BLEScan *pBLEScan;

const int scanTimeSeconds = 1;

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        if (strcmp(advertisedDevice.getName().c_str(), "63686f636f20636865736e6579") >= 0)
        {
            Serial.println(advertisedDevice.getRSSI());
        }
    }
};

void setup()
{
    Serial.begin(115200);
    Serial.println("Scanning...");

    BLEDevice::init("Radiation SCAN");
    pBLEScan = BLEDevice::getScan(); // create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(false); // active scan (true) uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99); // less or equal setInterval value

    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(LED_BUILTIN, ledChannel);
}

void loop()
{
    BLEScanResults foundDevices = pBLEScan->start(scanTimeSeconds, false);
    pBLEScan->clearResults();
}
#else
// define BTLE name
// CAREFUL: each character eats into your usable adv packet length
BLECast bleCast("RadiationBeacon");

uint8_t cnt = 0;
char data[5];

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE Beacon");

    bleCast.begin();
}

void loop()
{
    // note -- if you have too much data, it will not be added to the adv payload

    if (cnt == 20)
    {
        // reset
        cnt = 0;
    }

    if (cnt == 0)
    {
        // regenerate "random" data
        int red = random(20, 50);
        int orange = random(50, 70);
        sprintf(data, "%02d&%02d", red, orange);
    }
    cnt += 1;

    std::string s = bleCast.setManufacturerData(data, sizeof(data));
    Serial.println(s.c_str());
    delay(1000);
}
#endif