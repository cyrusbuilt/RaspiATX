/**
 * @file main.cpp
 * @author Cyrus Brunner (cyrusbuilt@gmail.com)
 * @brief Firmware for RaspiATX
 * @version 1.0
 * @date 2023-04-28
 * 
 * @copyright Copyright (c) Chris "Cyrus" Brunner 2023
 * 
 */

#include <Arduino.h>
#include "AtxManager.h"
#include "LED.h"

#define FIRMWARE_VERSION "1.0"
#define DEBUG_BAUD 9600

// Globals
HAF_LED pwrLED(PIN_PWR_LED, NULL);

void handleAtxPowerOn() {
    // PSU is on. Turn on chassis power LED.
    Serial.println(F("INIT: System initialized."));
    pwrLED.on();
}

void handleAtxPowerOff() {
    // PSU is off. Turn off chassis power LED.
    Serial.println(F("INFO: System shutdown detected."));
    pwrLED.off();
}

void handleAtxPowerInit() {
    Serial.println(F("INFO: System initializing..."));
}

void setup() {
    Serial.begin(DEBUG_BAUD);
    while (!Serial) {
        delay(10);
    }

    Serial.print(F("INIT: RaspiATX - PMU v"));
    Serial.println(FIRMWARE_VERSION);

    // Init chassis power LED.
    Serial.print(F("INIT: State LED... "));
    pwrLED.init();
    Serial.println(F("DONE"));

    // Init the ATX controller driver.
    Serial.print(F("INIT: ATX PMU controller... "));
    AtxController.begin();
    AtxController.onPowerInit(handleAtxPowerInit);
    AtxController.onPowerOn(handleAtxPowerOn);
    AtxController.onPowerOff(handleAtxPowerOff);
    Serial.println(F("DONE"));
}

void loop() {
    AtxController.loop();
}