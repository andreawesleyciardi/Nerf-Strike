#include "TargetPins.h"
#include <Arduino.h>
#include "TargetConfig.h"

TargetType targetType = TargetType::StrikeLite;

// 📟 7-Segment Display Instance
SevenSegmentDisplay scoreDisplay(displayDataPin, displayClockPin, displayLatchPin);

// 🟢 Button Instances
Button statusButton(statusButtonPin);
Button batteryButton(batteryButtonPin);

// 🌈 RGB LED Instances
RGBLed statusRgbLed(statusRedLedPin, statusGreenLedPin, statusBlueLedPin);
RGBLed batteryRgbLed(batteryRedLedPin, batteryGreenLedPin, batteryBlueLedPin);
RGBLed targetRgbLed(statusRedLedPin, statusGreenLedPin, statusBlueLedPin);
RGBLed entityRgbLed(batteryRedLedPin, batteryGreenLedPin, batteryBlueLedPin);
RGBRing rgbRing(ledRingPin, 24);

// Sensor Instance
Sensor sensor(sensorComponentPin);

// 🔊 Buzzer Instance
Buzzer buzzer(buzzerPin);

// 🔋 Battery Instance
Battery battery(true);

void initializeTargetPins() {
  // Battery
  battery.setup();

  // Buttons
  statusButton.setup();
  batteryButton.setup();
  sensor.setup();

  // RGB LEDs
  statusRgbLed.setup();
  batteryRgbLed.setup();
  targetRgbLed.setup();
  entityRgbLed.setup();
  rgbRing.setup();

  // Buzzer
  buzzer.setup();

  // 7-Segment Display
  scoreDisplay.setup(3);
}

void showAssignedIDBriefly(uint8_t id) {
  if (id != 0xFF) {
    scoreDisplay.showScore(id);
    delay(1000);
    scoreDisplay.clear();
  }
}