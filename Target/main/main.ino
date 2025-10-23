#include <Color.h>
#include <OPCodes.h>
#include <DisplayFeedback.h>
#include <Score.h>
#include <Statuses.h>

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

GameSessionStatus sessionStatus = GameSessionStatus::Setting;

String entityColorName;

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println(F("🔧 Target booting..."));

  randomSeed(analogRead(0));  // ✅ Better token randomness

  initializeTargetPins();
  wireless.setup();

  Serial.println(F("✅ Radio initialized and chip connected."));
  Serial.println(F("Radio listening on pairing pipe."));
  
  Serial.println(F("✅ Starting active pairing"));
  if (communication.activePairing()) {
    TargetInfo info = registry.getTargetInfo();
    targetRgbLed.blink(info.getColorName());
  }

  showAssignedIDBriefly(registry.getTargetInfo().id);

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
      showAssignedIDBriefly(registry.getTargetInfo().id);
    }
  }

  if (statusButton.wasLongPressed(feedback)) {
    Serial.println(F("🧹 Token reset triggered..."));
    registry.resetToken();
    showStatus(statusRgbLed, STATUS_OK, 3);
    delay(500);
    communication.activePairing();
    awaitingAck = false;
    lastHeartbeat = millis();
    heartbeatLost = false;
    showAssignedIDBriefly(registry.getTargetInfo().id);
  }

  if (wireless.available()) {
    byte buffer[32];
    wireless.read(buffer, sizeof(buffer));
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

    switch (header->opcode) {
      case OPCODE_HEARTBEAT: {
          lastHeartbeat = millis();
          heartbeatLost = false;
        break;
      }

      case OPCODE_BLINK_COMMAND: {
          Serial.println(F("🔦 Blink command received."));
          uint8_t assignedID = registry.getTargetInfo().id;
          scoreDisplay.showScore(assignedID);
          showStatus(statusRgbLed, STATUS_PAIRING);
          rgbRing.chase("Blue", 30);
          delay(1000);
          scoreDisplay.clear();
        break;
      }

      case OPCODE_ENTITY_COLOR: {
          entityColorName = communication.entityColor(buffer);
          if (entityColorName != "") {
            rgbRing.chase(entityColorName, 30);
            statusRgbLed.on(entityColorName, true);  // TEMPORARY: To add an "entityRgbLed"
            sessionStatus = GameSessionStatus::Setting;
            Serial.println(F("🌈 Entity color is set to: "));
            Serial.println(entityColorName);
          } else {
            Serial.println(F("❌ Was not possible to set the entity color"));
          }
        break;
      }

      case OPCODE_SCORE_UPDATE: {
          HitResponsePacket* packet = reinterpret_cast<HitResponsePacket*>(buffer);
          uint8_t newScore = packet->value;
          if (packet->value != 0xFF) {
            Serial.print(F("🎯 New score received: "));
            Serial.print(packet->value);
            Serial.print(F(" with status: "));
            Serial.println(static_cast<uint8_t>(packet->status));
            if ((packet->status == ScoreStatus::Add) || (packet->status == ScoreStatus::Subtract)) {
              if (packet->status == ScoreStatus::Add) {
                rgbRing.blink("Green");
              } else if (packet->status == ScoreStatus::Subtract) {
                rgbRing.blink("Orange");
              }
              scoreDisplay.updateScore(packet->value, false);
            }
            else {
              scoreDisplay.updateScore(packet->value, true);
              if (packet->status == ScoreStatus::Won || packet->status == ScoreStatus::Even) {
                rgbRing.chase("Green", 25);
                rgbRing.chase("Green", 25);
                rgbRing.blink("Green", 3);
                rgbRing.fill(entityColorName);
                delay(2000);
                rgbRing.off();
              } else if (packet->status == ScoreStatus::Lost) {
                rgbRing.chase("Red", 25);
                rgbRing.chase("Red", 25);
                rgbRing.blink("Red", 3);
              }
            }
            buzzer.beep(1);
          } else {
            showStatus(statusRgbLed, STATUS_ERROR);
            Serial.print(F("🎯 Was not possible to update the score"));
          }
        break;
      }

      case OPCODE_SESSION_STATUS: {
          // TO ADD AN ID TO THE SESSION AND THE LAST ON GOING SESSION CAN BE KEPT IN MEMORY SO IF IS LOST THE CONNECTION, ONCE FIXED CAN CHECK IF THE SESSION ON GOING IS STILL THE PREVIOUS ONE AND CAN THEN CATCH UP WHAT LOST.
          SessionStatusPacket* packet = reinterpret_cast<SessionStatusPacket*>(buffer);
          GameSessionStatus newStatus = packet->status;
          Serial.print(F("🔦 New GameSessionStatus received: "));
          Serial.println(GameSessionStatusToString(newStatus));
          if (newStatus == GameSessionStatus::Starting) {
            scoreDisplay.updateScore(0, true);
          }
          if (newStatus == GameSessionStatus::Paused) {
            entityRgbLed.startPulse(entityColorName);
          }
          if (sessionStatus == GameSessionStatus::Paused && (newStatus == GameSessionStatus::Playing || newStatus == GameSessionStatus::Ended)) {
            entityRgbLed.stopPulse();
            entityRgbLed.on();
          } else if (newStatus == GameSessionStatus::Ended || newStatus == GameSessionStatus::Setting) {
            entityRgbLed.off();
            rgbRing.off();
          }
          sessionStatus = newStatus;
        break;
      }

      default:
        Serial.print(F("⚠️ Unknown opcode received: "));
        Serial.println(header->opcode);
        break;
    }
  }

  //   if (wireless.send HitPacket(targetId)) {
  //     Serial.println(F("📡 Hit packet sent to hub."));
  //     awaitingAck = false;
  //   } else {
  //     Serial.println(F("⚠️ No response from hub. Will retry pairing."));
  //     awaitingAck = true;
  //   }
  //   lastHitTime = millis();
  // }

  // if (awaitingAck && millis() - lastHitTime > 5000) {
  //   Serial.println(F("🔄 Signal lost after hit. Re-pairing..."));
  //   communication.activePairing();
  //   awaitingAck = false;
  //   lastHeartbeat = millis();
  //   heartbeatLost = false;

  //   showAssignedIDBriefly(registry.getAssignedID());
  // }

  // if (millis() - lastHeartbeat > 10000 && !heartbeatLost) {                  // TO CHECK HOW TO REACTIVATE
  //   Serial.println(F("💔 No heartbeat. Re-pairing..."));
  //   heartbeatLost = true;
  //   showStatus(statusRgbLed, STATUS_DISCONNECTED);
  //   communication.activePairing();
  //   lastHeartbeat = millis();

  //   showAssignedIDBriefly(registry.getAssignedID());
  // }

  uint8_t targetId = registry.getTargetInfo().id;
  if (targetId != 0xFF) {
    if (sensor.isHit()) {
      if (sessionStatus == GameSessionStatus::Playing) {
        Serial.println(F("✅ Target got hit."));
        HitResponsePacket response = communication.hit();
      } else {
        Serial.println(F("✅ Target got hit while not playing."));
      }
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