#ifndef TARGET_CONFIG_H
#define TARGET_CONFIG_H

#include <stdint.h>

// Status RGB LED pins
const uint8_t statusRedLedPin   = 2;
const uint8_t statusGreenLedPin = 3;
const uint8_t statusBlueLedPin  = 4;

// Battery RGB LED pins
const uint8_t batteryRedLedPin   = 5;
const uint8_t batteryGreenLedPin = 6;
const uint8_t batteryBlueLedPin  = 7;

// RGB Led Ring
const uint8_t ledRingPin = 8;  // LED Ring

// NRF24L01 radio pins
const uint8_t transmitterCEPin = 9;
const uint8_t transmitterCSNPin = 10;

// Battery Button
const uint8_t batteryButtonPin = A0;

// 7-Segment Display via Shift Register
const uint8_t displayDataPin  = A1;  // SDI
const uint8_t displayClockPin = A2;  // SCLK
const uint8_t displayLatchPin = A3;  // LOAD

// Buzzer
const uint8_t buzzerPin = A4;

// Pairing Reset Button
const uint8_t pairingResetButtonPin = A5;

// Sensor (Piezo or Laser reader)
const uint8_t sensorComponentPin = A7;

const uint64_t pairingPipe = 0xF0F0F0F0E1LL;

struct PairingRequest {
  uint32_t token;
  bool hello;
};

struct PairingResponse {
  uint8_t assignedID;
};

#endif