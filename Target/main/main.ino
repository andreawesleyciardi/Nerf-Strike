#include <Color.h>
#include <OPCodes.h>
#include <DisplayFeedback.h>
#include "TargetConfig.h"
#include "TargetPins.h"
#include "WirelessTarget.h"
#include "Send.h"
#include "PairingRegistry.h"

// Core managers
WirelessTarget wireless;
Send send(wireless.getRadio());
PairingRegistry registry(wireless, send);

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println(F("🔧 Target booting..."));

  randomSeed(analogRead(0));  // ✅ Better token randomness

  initializeTargetPins();
  wireless.setup();

  Serial.println(F("✅ Radio initialized and chip connected."));
  Serial.println(F("Radio listening on pairing pipe."));

  registry.pair();  // Stateless pairing

  showAssignedIDBriefly(registry.getAssignedID());

  Serial.println(F("Setup complete."));
}

void loop() {
  static unsigned long lastHitTime = 0;
  static unsigned long lastHeartbeat = millis();
  static bool awaitingAck = false;
  static bool heartbeatLost = false;

  battery.autoCheck(batteryVerificationFeedback);
  if (batteryButton.wasPressed()) {
    battery.check(batteryVerificationFeedback);
  }

  if (statusButton.wasPressed()) {
    Serial.println(F("🔍 Verifying connection with hub..."));
    bool verified = registry.verify();
    if (verified) {
      Serial.println(F("✅ Connection verified."));
      showStatus(statusRgbLed, STATUS_CONNECTED);
    } else {
      Serial.println(F("❌ Verification failed. Re-pairing..."));
      showStatus(statusRgbLed, STATUS_ERROR);
      registry.pair();
    }
    showAssignedIDBriefly(registry.getAssignedID());
  }

  if (statusButton.wasLongPressed(feedback)) {
    Serial.println(F("🧹 Token reset triggered..."));
    registry.resetToken();
    showStatus(statusRgbLed, STATUS_OK, 3);
    delay(500);
    registry.pair();
    awaitingAck = false;
    lastHeartbeat = millis();
    heartbeatLost = false;
    showAssignedIDBriefly(registry.getAssignedID());
  }

  if (wireless.available()) {
    byte buffer[32];
    wireless.read(buffer, sizeof(buffer));
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

    switch (header->opcode) {
      case OPCODE_HEARTBEAT: {
          Serial.println(F("💓 Heartbeat received from hub."));
          lastHeartbeat = millis();
          heartbeatLost = false;
        break;
      }

      case OPCODE_BLINK_COMMAND: {
          Serial.println(F("🔦 Blink command received."));
          uint8_t assignedID = registry.getAssignedID();
          scoreDisplay.showScore(assignedID);
          showStatus(statusRgbLed, STATUS_PAIRING);
          rgbRing.chase("Blue", 30);
          delay(1000);
          scoreDisplay.clear();
        break;
      }

      case OPCODE_SCORE_UPDATE: {
          ScoreUpdatePacket* packet = reinterpret_cast<ScoreUpdatePacket*>(buffer);
          uint8_t newScore = packet->score;
          Serial.print(F("🎯 New score received: "));
          Serial.println(newScore);
          scoreDisplay.updateScore(newScore);
        break;
      }

      default:
        Serial.print(F("⚠️ Unknown opcode received: "));
        Serial.println(header->opcode);
        break;
    }
  }

  uint8_t targetId = registry.getAssignedID();
  if (targetId != 0xFF) {
    if (sensor.isHit()) {
      Serial.println(F("✅ Target got hit."));
      rgbRing.blink("Green");
      buzzer.beep(10);

      if (wireless.sendHitPacket(targetId)) {
        Serial.println(F("📡 Hit packet sent to hub."));
        awaitingAck = false;
      } else {
        Serial.println(F("⚠️ No response from hub. Will retry pairing."));
        awaitingAck = true;
      }
      lastHitTime = millis();
    }

    if (awaitingAck && millis() - lastHitTime > 5000) {
      Serial.println(F("🔄 Signal lost after hit. Re-pairing..."));
      registry.pair();
      awaitingAck = false;
      lastHeartbeat = millis();
      heartbeatLost = false;

      showAssignedIDBriefly(registry.getAssignedID());
    }

    if (millis() - lastHeartbeat > 10000 && !heartbeatLost) {
      Serial.println(F("💔 No heartbeat. Re-pairing..."));
      heartbeatLost = true;
      showStatus(statusRgbLed, STATUS_DISCONNECTED);
      registry.pair();
      lastHeartbeat = millis();

      showAssignedIDBriefly(registry.getAssignedID());
    }
  }
}

void feedback() {
  showStatus(statusRgbLed, STATUS_ERROR);
}

void batteryVerificationFeedback(long batteryValue, bool wasAutomaticCheck) {
  if (batteryValue < 30) {
    if (batteryValue < 10) {
      showStatus(batteryRgbLed, STATUS_CRITICAL_BATTERY);
    } else {
      showStatus(batteryRgbLed, STATUS_LOW_BATTERY);
    }
  } else if (batteryValue < 60) {
    if (!wasAutomaticCheck) showStatus(batteryRgbLed, STATUS_LOW_BATTERY);
  } else {
    if (!wasAutomaticCheck) showStatus(batteryRgbLed, STATUS_OK);
  }
}