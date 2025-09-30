#ifndef HUB_CONFIG_H
#define HUB_CONFIG_H

#include <stdint.h>

#define MAX_TARGETS 10

// 📡 Wireless Communication
const uint8_t transmitterCEPin = 9;
const uint8_t transmitterCSNPin = 10;
const uint64_t pairingPipe = 0xF0F0F0F0E1LL;

// 📟 7-Segment Display (Shift Register Interface)
const uint8_t segmentSDIPin   = 26;
const uint8_t segmentSCLKPin  = 27;
const uint8_t segmentLOADPin  = 28;

// 🖥️ 20x4 LCD (I2C)
const uint8_t lcdSDAPin = SDA;   // Communication pin SDA (pin 20)
const uint8_t lcdSCLPin = SCL;   // Communication pin SCL (pin 21)

// 🎛️ Rotary Encoder
const uint8_t encoderCLKPin = 2;
const uint8_t encoderDTPin  = 3;
const uint8_t encoderSWPin  = 4;

// 🔘 Navigation Buttons
const uint8_t buttonCancelPin = 7;  // Left of encoder
const uint8_t buttonSavePin   = 8;  // Right of encoder

// 🔋 Functional Buttons
const uint8_t batteryButtonPin = 6;
const uint8_t pairingResetButtonPin = 5;

// 🌈 RGB LED – Pairing Status
const uint8_t pairingLedRedPin   = 25;
const uint8_t pairingLedGreenPin = 21;
const uint8_t pairingLedBluePin  = 23;

// 🌈 RGB LED – Battery Status
const uint8_t batteryLedRedPin   = 24;
const uint8_t batteryLedGreenPin = 20;
const uint8_t batteryLedBluePin  = 22;

// 🌈 LED Strip
const uint8_t ledStripPin = 45;

// 🔊 MP3 Decoder (Serial2)
const uint8_t mp3RxPin = 16;  // TX2
const uint8_t mp3TxPin = 17;  // RX2

// 🌐 ESP8266 WiFi Module (Serial1)
const uint8_t espRxPin = 18;  // TX1
const uint8_t espTxPin = 19;  // RX1

// 💾 Optional SD Card (SPI)
const uint8_t sdCardCSPin = 53;

// 📦 Pairing Structures
struct PairingRequest {
  uint32_t token;
  bool hello;
};

struct PairingResponse {
  uint8_t assignedID;
};

#endif