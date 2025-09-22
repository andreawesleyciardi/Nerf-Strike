#ifndef TARGET_CONFIG_H
#define TARGET_CONFIG_H

#include <stdint.h>

// NRF24L01 radio pins
const uint8_t transmitterCEPin = 9;
const uint8_t transmitterCSNPin = 10;

// Piezo sensor
const uint8_t piezoSensorPin = A0; // Piezo Sensor

// Pairing reset button
const uint8_t pairingResetPin = A1;

// RGB LED pins
const uint8_t redLEDPin   = 8;
const uint8_t greenLEDPin = 6;
const uint8_t blueLEDPin  = 7;

// 7-Segment Display via Shift Register
const uint8_t displayLatchPin = 4;  // LOAD
const uint8_t displayClockPin = 3;  // SCLK
const uint8_t displayDataPin  = 2;  // SDI

// RGB Ring
const uint8_t ledRingPin = 5;  // LED Ring

const uint64_t pairingPipe = 0xF0F0F0F0E1LL;

struct PairingRequest {
  uint32_t token;
  bool hello;
};

struct PairingResponse {
  uint8_t assignedID;
};

#endif