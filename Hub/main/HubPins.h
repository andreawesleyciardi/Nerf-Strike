#ifndef HUB_PINS_H
#define HUB_PINS_H

#include <Button.h>
#include <RGBLed.h>
#include <Battery.h>
#include <SevenSegmentDisplay.h>
#include <RotaryEncoder.h>

// 📟 7-Segment Display Instance
extern SevenSegmentDisplay timeDisplay;

// 🟢 Button Instances
extern Button leftButton;
extern Button rightButton;
extern Button batteryButton;
extern Button statusButton;

// 🌈 RGB LED Instances
extern RGBLed statusRgbLed;
extern RGBLed batteryRgbLed;

// 🔋 Battery Instance
extern Battery battery;

// 🎛️ Rotary Encoder Instance
extern RotaryEncoder encoder;

// 🔧 Initialization Function
void initializeHubPins();

#endif