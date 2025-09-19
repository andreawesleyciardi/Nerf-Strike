#include <EEPROM.h>
#include "TargetConfig.h"
#include "WirelessManager.h"
#include "PairingManager.h"

WirelessManager wireless;
PairingManager pairingManager(wireless);

void setup() {
  Serial.begin(9600);
  Serial.println("Target starting...");

  pinMode(pairingResetPin, INPUT_PULLUP);
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);

  wireless.initialize();

  Serial.println("✅ Radio initialized and chip connected.");
  Serial.println("Radio listening on pairing pipe.");

  uint8_t pairingFlag = EEPROM.read(0);
  uint8_t assignedID = EEPROM.read(1);

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
  if (digitalRead(pairingResetPin) == LOW) {
    EEPROM.write(0, 0); // Clear pairing flag
    EEPROM.write(1, 0); // Clear assigned ID
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
      blinkRGB(3);
    }
  }

  // Add any runtime logic here (e.g., hit detection, LED updates)
}

void blinkRGB(uint8_t times) {
  for (uint8_t i = 0; i < times; i++) {
    setRGB(255, 255, 255); // white
    delay(200);
    setRGB(0, 0, 0);       // off
    delay(200);
  }
}

void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(redLEDPin, r);
  analogWrite(greenLEDPin, g);
  analogWrite(blueLEDPin, b);
}
