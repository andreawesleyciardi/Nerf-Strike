#include <Color.h>
#include <OPCodes.h>
#include <DisplayFeedback.h>
#include "TargetConfig.h"
#include "TargetPins.h"
#include "WirelessTarget.h"
#include "PairingRegistry.h"
#include "Send.h"
#include "Receive.h"
#include "Communication.h"

// 🧠 Core Managers
WirelessTarget wireless;
PairingRegistry registry(wireless.getRadio());
Receive receive(wireless.getRadio(), registry);
Send send(wireless.getRadio(), registry);

Communication communication(receive, send, registry, statusRgbLed);

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println(F("🔧 Target booting..."));

  randomSeed(analogRead(0));  // ✅ Better token randomness

  initializeTargetPins();
  wireless.setup();

  Serial.println(F("✅ Radio initialized and chip connected."));
  Serial.println(F("Radio listening on pairing pipe."));

  // registry.pair();  // Stateless pairing
  communication.pairing();

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
    if (communication.verification()) {
      showAssignedIDBriefly(registry.getAssignedID());
    }
  }

  if (statusButton.wasLongPressed(feedback)) {
    Serial.println(F("🧹 Token reset triggered..."));
    registry.resetToken();
    showStatus(statusRgbLed, STATUS_OK, 3);
    delay(500);
    communication.pairing();
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
          // Serial.println(F("💓 Heartbeat received from hub."));                  // To restore
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

      case OPCODE_ENTITY_COLOR: {
          String colorName = communication.entityColor(buffer);
          if (colorName != "") {
            rgbRing.chase(colorName, 30);
            Serial.println(F("🌈 Entity color is set to: "));
            Serial.println(colorName);
          } else {
            Serial.println(F("❌ Was not possible to set the entity color"));
          }
        break;
      }

      // case OPCODE_SCORE_UPDATE: {
      //     ScoreUpdatePacket* packet = reinterpret_cast<ScoreUpdatePacket*>(buffer);
      //     uint8_t newScore = packet->score;
      //     Serial.print(F("🎯 New score received: "));
      //     Serial.println(newScore);
      //     scoreDisplay.updateScore(newScore);
      //   break;
      // }

      default:
        Serial.print(F("⚠️ Unknown opcode received: "));
        Serial.println(header->opcode);
        break;
    }
  }

  uint8_t targetId = registry.getAssignedID();
  if (targetId != 0xFF) {
    if (sensor.isHit()) {
      // TO CREATE CONDITIONS: if (gameStatus == PLAYING) {
        Serial.println(F("✅ Target got hit."));
        HitResponsePacket response = communication.hit();
        if (response.score != 0xFF) {
          rgbRing.blink("Green");
          scoreDisplay.updateScore(response.score);
          buzzer.beep(2);
          Serial.print(F("🎯 New score received: "));
          Serial.println(response.score);
          // feedback.updateScore(response.score);
        } else {
          showStatus(statusRgbLed, STATUS_ERROR);
          Serial.print(F("🎯 Was not possible to update the score"));
          // feedback.signalError();  // Optional: blink red or buzz
        }
      // TO CREATE CONDITIONS: }

      

    //   if (wireless.send HitPacket(targetId)) {
    //     Serial.println(F("📡 Hit packet sent to hub."));
    //     awaitingAck = false;
    //   } else {
    //     Serial.println(F("⚠️ No response from hub. Will retry pairing."));
    //     awaitingAck = true;
    //   }
    //   lastHitTime = millis();
    }

    // if (awaitingAck && millis() - lastHitTime > 5000) {
    //   Serial.println(F("🔄 Signal lost after hit. Re-pairing..."));
    //   communication.pairing();
    //   awaitingAck = false;
    //   lastHeartbeat = millis();
    //   heartbeatLost = false;

    //   showAssignedIDBriefly(registry.getAssignedID());
    // }

    // if (millis() - lastHeartbeat > 10000 && !heartbeatLost) {                  // TO CHECK HOW TO REACTIVATE
    //   Serial.println(F("💔 No heartbeat. Re-pairing..."));
    //   heartbeatLost = true;
    //   showStatus(statusRgbLed, STATUS_DISCONNECTED);
    //   communication.pairing();
    //   lastHeartbeat = millis();

    //   showAssignedIDBriefly(registry.getAssignedID());
    // }
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