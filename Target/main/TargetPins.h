#ifndef TARGET_PINS_H
#define TARGET_PINS_H

#include <Button.h>
#include <RGBLed.h>
#include <Battery.h>
#include "TargetConfig.h"
#include "RGBRing.h"
#include "Sensor.h"
#include "SevenSegmentDisplay.h"
#include "Buzzer.h"

// 📟 7-Segment Display Instance
extern SevenSegmentDisplay scoreDisplay;

// 🌈 RGB LED Instances
extern RGBLed statusRgbLed;
extern RGBLed batteryRgbLed;
extern RGBLed targetRgbLed;
extern RGBLed entityRgbLed;
extern RGBRing rgbRing;

// Sensor Instance
extern Sensor sensor;

// 🟢 Button Instances
extern Button statusButton;
extern Button batteryButton;

// 🔊 Buzzer Instance
extern Buzzer buzzer;

// 🔋 Battery Instance
extern Battery battery;

// Setup function
void initializeTargetPins();

void showAssignedIDBriefly(uint8_t id);

#endif