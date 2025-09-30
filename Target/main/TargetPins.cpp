#include "TargetPins.h"
#include <Arduino.h>

// Instantiate components
RGBLed statusRgbLed(statusRedLedPin, statusGreenLedPin, statusBlueLedPin);
RGBLed batteryRgbLed(batteryRedLedPin, batteryGreenLedPin, batteryBlueLedPin);
RGBRing rgbRing(ledRingPin, 24);
Sensor sensor(sensorComponentPin);
SevenSegmentDisplay scoreDisplay(displayDataPin, displayClockPin, displayLatchPin);
Button pairingResetButton(pairingResetButtonPin);
Button batteryButton(batteryButtonPin);
Buzzer buzzer(buzzerPin);
Battery battery(true);

void initializeTargetPins() {
  battery.setup();
  pairingResetButton.setup();
  batteryButton.setup();
  sensor.setup();
  statusRgbLed.setup();
  batteryRgbLed.setup();
  rgbRing.setup();
  buzzer.setup();
  scoreDisplay.setup(3);
}

void showAssignedIDBriefly(uint8_t id) {
  if (id != 0xFF) {
    scoreDisplay.showScore(id);
    delay(1000);
    scoreDisplay.clear();
  }
}