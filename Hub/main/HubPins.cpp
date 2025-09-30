#include "HubPins.h"
#include "HubConfig.h"
#include <Arduino.h>

// 🟢 Button Instances
Button buttonCancel(buttonCancelPin, 50, 1000);
Button buttonSave(buttonSavePin, 50, 1000);
Button batteryButton(batteryButtonPin, 50, 1000);
Button pairingButton(pairingResetButtonPin, 50, 1000);

// 🌈 RGB LED Instances
RGBLed pairingLed(pairingLedRedPin, pairingLedGreenPin, pairingLedBluePin);
RGBLed batteryLed(batteryLedRedPin, batteryLedGreenPin, batteryLedBluePin);

void initializeHubPins() {
  // Buttons
  buttonCancel.setup();
  buttonSave.setup();
  batteryButton.setup();
  pairingButton.setup();

  // RGB LEDs
  pairingLed.setup(false);
  batteryLed.setup(false);

  // Rotary Encoder
  pinMode(encoderCLKPin, INPUT);
  pinMode(encoderDTPin, INPUT);
  pinMode(encoderSWPin, INPUT_PULLUP);

  // LED Strip
  pinMode(ledStripPin, OUTPUT);

  // 7-Segment Display
  pinMode(segmentSDIPin, OUTPUT);
  pinMode(segmentSCLKPin, OUTPUT);
  pinMode(segmentLOADPin, OUTPUT);

  // Serial Devices
  Serial1.begin(9600); // ESP8266
  Serial2.begin(9600); // MP3 Decoder
}