#include <EEPROM.h>
#include <Color.h>
#include "TargetConfig.h"
#include "WirelessManager.h"
#include "PairingManager.h"
#include "Piezo.h"
#include <RGBLed.h>
#include "RGBRing.h"
#include "SevenSegmentDisplay.h"
#include <Button.h>

// Components
WirelessManager wireless;
PairingManager pairingManager(wireless);
RGBLed rgbLed(redLEDPin, greenLEDPin, blueLEDPin);
RGBRing rgbRing(ledRingPin, 24);
Piezo piezo(piezoSensorPin);
SevenSegmentDisplay scoreDisplay(displayDataPin, displayClockPin, displayLatchPin);
Button pairingResetButton(pairingResetPin);

void setup() {
  Serial.begin(9600);
  Serial.println("Target starting...");

  pairingResetButton.setup();
  piezo.setup();
  rgbLed.setup();
  rgbRing.setup();
  scoreDisplay.setup();

  wireless.setup();

  Serial.println("✅ Radio initialized and chip connected.");
  Serial.println("Radio listening on pairing pipe.");

  uint8_t pairingFlag = pairingManager.readFlag();
  uint8_t assignedID = pairingManager.readId();

  if (pairingFlag == 1 && assignedID != 0xFF) {
    Serial.print("✅ Found stored ID: ");
    Serial.println(assignedID);
    if (pairingManager.verifyPairing(assignedID)) {
      Serial.println("✅ Verified with hub.");
    } else {
      Serial.println("⚠️ Hub did not recognize ID. Re-pairing...");
      pairingManager.pair();
    }
  } else {
    Serial.println("🔍 No valid pairing found. Attempting to pair...");
    pairingManager.pair();
  }

  Serial.println("Setup complete.");
}

void loop() {
  if (pairingResetButton.wasPressed()) {
    pairingManager.clear();
    Serial.println("🔄 EEPROM reset. Re-pairing...");

    // Blink green LED 3 times
    for (int i = 0; i < 3; i++) {
      digitalWrite(greenLEDPin, HIGH);
      delay(200);
      digitalWrite(greenLEDPin, LOW);
      delay(200);
    }

    delay(500); // Debounce

    pairingManager.pair(); // Re-pair immediately
  }

  // ✅ Listen for blink command from hub
  if (wireless.available()) {
    byte packet[32];
    wireless.read(packet, sizeof(packet));

    if (packet[0] == 0x05) { // Blink command
      Serial.println("🔦 Blink command received.");
      rgbLed.blink("Green");
    }
  }

  if (pairingManager.isPaired()) {
    if (piezo.isHit()) {
      // ✅ Trigger feedback
      Serial.println("✅ Target got hit.");
      rgbRing.chase("Green", 30);
      // To-Do: Emit Sound                                      <-------------
      uint8_t targetId = pairingManager.readId();
      uint8_t score;
      if (wireless.sendHitPacket(targetId, score)) {
        Serial.print("✅ Score: ");
        Serial.println(score);
        scoreDisplay.showScore(score);
      }
    }
    // Add any runtime logic here (e.g., hit detection, LED updates)
  }
}