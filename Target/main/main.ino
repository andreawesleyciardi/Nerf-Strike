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
  delay(1000);
  Serial.begin(9600);
  Serial.println(F("🔧 Target booting..."));

  randomSeed(analogRead(0));  // ✅ Better token randomness

  pairingResetButton.setup();
  piezo.setup();
  rgbLed.setup();
  rgbRing.setup();
  scoreDisplay.setup();
  wireless.setup();

  Serial.println(F("✅ Radio initialized and chip connected."));
  Serial.println(F("Radio listening on pairing pipe."));

  pairingManager.pair();  // Stateless pairing

  // 🧠 Show pairing status on display
  uint8_t id = pairingManager.getAssignedID();
  if (id != 0xFF) {
    scoreDisplay.showScore(id);
    delay(1000);
    scoreDisplay.clear();
  }

  Serial.println("Setup complete.");
}

void loop() {
  static unsigned long lastHitTime = 0;
  static unsigned long lastHeartbeat = millis();
  static bool awaitingAck = false;
  static bool heartbeatLost = false;

  if (pairingResetButton.wasPressed()) {
    Serial.println(F("🔄 Manual re-pair triggered..."));
    rgbLed.blink("Red");
    delay(500);
    pairingManager.pair();
    awaitingAck = false;
    lastHeartbeat = millis();
    heartbeatLost = false;

    uint8_t id = pairingManager.getAssignedID();
    if (id != 0xFF) {
      scoreDisplay.showScore(id);
      delay(1000);
      scoreDisplay.clear();
    }
  }

  if (wireless.available()) {
    // Serial.println("📥 Target received a packet.");

    byte packet[32];
    wireless.read(packet, sizeof(packet));

    if (packet[0] == 0x04) { // 💓 Heartbeat ping
      lastHeartbeat = millis();
      heartbeatLost = false;
      Serial.println(F("💓 Heartbeat received from hub."));
    }

    if (packet[0] == 0x05) { // 🔦 Blink command
      Serial.println(F("🔦 Blink command received."));
      uint8_t assignedID = pairingManager.getAssignedID();
      scoreDisplay.showScore(assignedID);
      rgbLed.blink("Blue");
      rgbRing.chase("Blue", 30);
      delay(1000);
      scoreDisplay.clear();
    }

    if (packet[0] == 0x06) { // 🎯 Score update
      uint8_t newScore = packet[1];
      Serial.print(F("🎯 New score received: "));
      Serial.println(newScore);
      scoreDisplay.updateScore(newScore);
    }
  }

  uint8_t targetId = pairingManager.getAssignedID();
  if (targetId != 0xFF) {
    if (piezo.isHit()) {
      Serial.println(F("✅ Target got hit."));
      rgbRing.chase("Green", 30);

      if (wireless.sendHitPacket(targetId)) {
        Serial.println(F("📡 Hit packet sent to hub."));
        lastHitTime = millis();
        awaitingAck = false;
      } else {
        Serial.println(F("⚠️ No response from hub. Will retry pairing."));
        awaitingAck = true;
        lastHitTime = millis();
      }
    }

    if (awaitingAck && millis() - lastHitTime > 5000) {
      Serial.println(F("🔄 Signal lost after hit. Re-pairing..."));
      pairingManager.pair();
      awaitingAck = false;
      lastHeartbeat = millis();
      heartbeatLost = false;

      uint8_t id = pairingManager.getAssignedID();
      if (id != 0xFF) {
        scoreDisplay.showScore(id);
        delay(1000);
        scoreDisplay.clear();
      }
    }

    if (millis() - lastHeartbeat > 10000 && !heartbeatLost) {
      Serial.println(F("💔 No heartbeat. Re-pairing..."));
      heartbeatLost = true;
      rgbLed.blink("Red");
      pairingManager.pair();
      lastHeartbeat = millis();

      uint8_t id = pairingManager.getAssignedID();
      if (id != 0xFF) {
        scoreDisplay.showScore(id);
        delay(1000);
        scoreDisplay.clear();
      }
    }
  }
}
