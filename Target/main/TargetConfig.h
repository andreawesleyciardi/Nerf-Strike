#ifndef TARGET_CONFIG_H
#define TARGET_CONFIG_H

#include <stdint.h>
#include <TargetType.h>

extern TargetType targetType;

// 📡 Wireless Communication - NRF24L01
const uint8_t transmitterCEPin = 9;
const uint8_t transmitterCSNPin = 10;
const uint64_t pairingPipe = 0xF0F0F0F0E1LL;

// 📟 7-Segment Display (Shift Register Interface)
const uint8_t displayLatchPin = A1;  // LOAD
const uint8_t displayClockPin = A2;  // SCLK
const uint8_t displayDataPin  = A3;  // SDI

// 🔋 Functional Buttons
const uint8_t batteryButtonPin = A0;
const uint8_t statusButtonPin = A5;

// 🌈 RGB LED – Status
const uint8_t statusRedLedPin   = 2;
const uint8_t statusGreenLedPin = 3;
const uint8_t statusBlueLedPin  = 4;

// 🌈 RGB LED – Battery
const uint8_t batteryRedLedPin   = 5;
const uint8_t batteryGreenLedPin = 6;
const uint8_t batteryBlueLedPin  = 7;

// 🌈 RGB Led Ring
const uint8_t ledRingPin = 8;  // LED Ring

// 🔊 Buzzer
const uint8_t buzzerPin = A4;

// Sensor (Piezo or Laser reader)
const uint8_t sensorComponentPin = A7;


// 📦 Pairing Structures
struct PairingRequest {
  uint32_t token;
  bool hello;
};

struct PairingResponse {
  uint8_t opcode;
  uint8_t assignedID;
  TargetType type;
};

#endif