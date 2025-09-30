#ifndef HUB_PINS_H
#define HUB_PINS_H

#include "Button.h"
#include "RGBLed.h"

// 🟢 Button Instances
extern Button buttonCancel;
extern Button buttonSave;
extern Button batteryButton;
extern Button pairingButton;

// 🌈 RGB LED Instances
extern RGBLed pairingLed;
extern RGBLed batteryLed;

// 🔧 Initialization Function
void initializeHubPins();

#endif