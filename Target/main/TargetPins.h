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

// Shared instances
extern RGBLed statusRgbLed;
extern RGBLed batteryRgbLed;
extern RGBRing rgbRing;
extern Sensor sensor;
extern SevenSegmentDisplay scoreDisplay;
extern Button pairingResetButton;
extern Button batteryButton;
extern Buzzer buzzer;
extern Battery battery;

// Setup function
void initializeTargetPins();

void showAssignedIDBriefly(uint8_t id);

#endif