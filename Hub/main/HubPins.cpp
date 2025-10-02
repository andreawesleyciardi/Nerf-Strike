#include "HubPins.h"
#include "HubConfig.h"
#include <Arduino.h>

// 📟 7-Segment Display Instance
SevenSegmentDisplay timeDisplay(displayDataPin, displayClockPin, displayLatchPin);

// 🟢 Button Instances
Button leftButton(leftButtonPin, 50, 1000);
Button rightButton(rightButtonPin, 50, 1000);
Button batteryButton(batteryButtonPin, 50, 1000);
Button statusButton(pairingResetButtonPin, 50, 1000);

// 🌈 RGB LED Instances
RGBLed statusRgbLed(statusRgbLedRedPin, statusRgbLedGreenPin, statusRgbLedBluePin);
RGBLed batteryRgbLed(batteryRgbLedRedPin, batteryRgbLedGreenPin, batteryRgbLedBluePin);

// 🔋 Battery Instance
Battery battery(true);

// 🎛️ Rotary Encoder Instance
RotaryEncoder encoder(encoderCLKPin, encoderDTPin, encoderSWPin);

void initializeHubPins() {
  // Battery
  battery.setup();

  // Buttons
  leftButton.setup();
  rightButton.setup();
  statusButton.setup();
  batteryButton.setup();

  // RGB LEDs
  statusRgbLed.setup(false);
  batteryRgbLed.setup(false);

  // Rotary Encoder
  encoder.setup();

  // LED Strip
  pinMode(ledStripPin, OUTPUT);

  // 7-Segment Display
  timeDisplay.setup(4);

  // Serial Devices
  Serial1.begin(9600); // ESP8266
  Serial2.begin(9600); // MP3 Decoder
}