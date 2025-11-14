#include <Color.h>
#include <ColorPalette.h>
#include <OPCodes.h>
#include <DisplayFeedback.h>
#include <Score.h>
#include <Statuses.h>
#include <Target.h>

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

// String entityColorName;

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("🚀🚀🚀🚀🚀🚀🚀"));
  Serial.println(F("🔧 Target booting..."));

  randomSeed(analogRead(0));  // ✅ Better token randomness

  initializeTargetPins();
  wireless.setup();

  Serial.println();
  Serial.println(F("📡 Radio initialized and chip connected."));
  Serial.println(F("📡 Radio listening on pairing pipe."));
  
  Serial.println();
  Serial.println(F("🛰️ Starting active pairing"));
  if (communication.pairingRequest()) {
    TargetInfo info = registry.getTargetInfo();
    registry.switchToTargetPipe(info.id);
    targetRgbLed.setPrimaryColorName(info.getColorName());
    targetRgbLed.blink();
    showAssignedIDBriefly(registry.getTargetInfo().id);
  }
  else {
    Serial.println();
    Serial.println(F("❌ Active pairing failed."));
    showStatus(statusRgbLed, STATUS_DISCONNECTED);
  }

  Serial.println(F("🔧 Setup complete."));
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
    registry.resetTargetInfoEEPROM();
    showStatus(statusRgbLed, STATUS_OK, 3);
    delay(500);
    communication.pairingRequest();
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
      case OPCODE_PAIRING_SOLLECITATION: {
          Serial.println();
          Serial.println(F("📡 Hub sent a pairing sollecitation. Replying with pairing request."));
          delay(50);
          communication.pairingRequest();
        break;
      }

      case OPCODE_HEARTBEAT_REQUEST: {
          lastHeartbeat = millis();
          heartbeatLost = false;
          // communication.heartbeat();
        break;
      }

      case OPCODE_BLINK_COMMAND: {
          Serial.println(F("🔦 Blink command received."));
          TargetInfo info = registry.getTargetInfo();
          scoreDisplay.showScore(info.id);
          showStatus(statusRgbLed, STATUS_PAIRING);
          rgbRing.chase(info.getColorName(), 30);
          delay(1000);
          scoreDisplay.clear();
        break;
      }

      case OPCODE_TARGET_SESSION_INFO: {
          TargetSessionInfoRequestPacket* packet = reinterpret_cast<TargetSessionInfoRequestPacket*>(buffer);
          TargetSessionInfo sessionInfo = packet->sessionInfo;

          TargetInfo info = registry.getTargetInfo();
          info.enabled = sessionInfo.enabled;
          info.indexInEntity = sessionInfo.indexInEntity;
          info.entityColorIndex = sessionInfo.entityColorIndex;

          registry.setTargetInfo(info);

          // ✅ Debug print
          Serial.print(F("📤 Received TargetSessionInfo in Target ID: "));
          Serial.println(info.id);
          Serial.print(F("  - enabled: "));
          Serial.println(info.enabled ? "true" : "false");
          Serial.print(F("  - indexInEntity: "));
          Serial.println(info.indexInEntity);
          Serial.print(F("  - entityColorIndex: "));
          Serial.println(info.entityColorIndex);

          if (info.enabled == true) {
            Color entityColor = ColorPalette::getByIndex(info.entityColorIndex);
            scoreDisplay.showScore(info.indexInEntity);
            rgbRing.chase(entityColor.name, 30);
            entityRgbLed.on(entityColor.name, true);
            Serial.println(F("✅ The Target is enabled and correctly set."));
          }
          else {
            entityRgbLed.off();
            targetRgbLed.off();
            statusRgbLed.on("White");
            Serial.println(F("❌ The Target is disabled."));
          }
        break;
      }

      case OPCODE_IDENTIFY_TARGET: {
          bool switchOn = communication.identifyTarget(buffer);
          if (switchOn == true) {
            targetRgbLed.on();
            Serial.println(F("🌈 Target rgb led is ON."));
            TargetInfo info = registry.getTargetInfo();
            scoreDisplay.showScore(info.id);
          } else {
            targetRgbLed.off();
            Serial.println(F("❌ Target rgb led is OFF."));
            scoreDisplay.showScore(0);
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
                TargetInfo info = registry.getTargetInfo();
                Color entityColor = ColorPalette::getByIndex(info.entityColorIndex);
                rgbRing.chase(packet->status == ScoreStatus::Won ? "Green" : "Orange", 25);
                rgbRing.chase(packet->status == ScoreStatus::Won ? "Green" : "Orange", 25);
                rgbRing.blink(packet->status == ScoreStatus::Won ? "Green" : "Orange", 3);
                rgbRing.fill(entityColor.name);
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
          // TO DO MAYBE: ADD AN ID TO THE SESSION AND THE LAST ON GOING SESSION CAN BE KEPT IN MEMORY SO IF IS LOST THE CONNECTION, ONCE FIXED CAN CHECK IF THE SESSION ON GOING IS STILL THE PREVIOUS ONE AND CAN THEN CATCH UP WHAT LOST.
          SessionStatusPacket* packet = reinterpret_cast<SessionStatusPacket*>(buffer);
          GameSessionStatus newStatus = packet->status;
          Serial.print(F("🔦 New GameSessionStatus received: "));
          Serial.println(GameSessionStatusToString(newStatus));
          if (newStatus == GameSessionStatus::Starting || newStatus == GameSessionStatus::Resetting) {
            scoreDisplay.updateScore(0, true);
          }
          TargetInfo info = registry.getTargetInfo();
          if (newStatus == GameSessionStatus::Paused) {
            Color entityColor = ColorPalette::getByIndex(info.entityColorIndex);
            entityRgbLed.startPulse(entityColor.name);
          }
          targetRgbLed.off();
          if (sessionStatus == GameSessionStatus::Paused && (newStatus == GameSessionStatus::Playing || newStatus == GameSessionStatus::Ended)) {
            entityRgbLed.stopPulse();
            entityRgbLed.on();
          } else if (newStatus == GameSessionStatus::Ended || newStatus == GameSessionStatus::Resetting) {
            entityRgbLed.off();
            rgbRing.off();
            delay(500);
            scoreDisplay.updateScore(0, true);
          } else if (newStatus == GameSessionStatus::Setting) {
            entityRgbLed.off();
            targetRgbLed.on();
            rgbRing.off();
            delay(500);
            scoreDisplay.updateScore(info.id, true);
          }
          // To check if this happens already:
          // if (newStatus == GameSessionStatus::Playing) {
          //   entityRgbLed.on();
          // }
          sessionStatus = newStatus;
        break;
      }

      case OPCODE_GAMEMODE_LITTARGET: {
          GameModeLitTargetRequestPacket* packet = reinterpret_cast<GameModeLitTargetRequestPacket*>(buffer);
          uint8_t indexInEntity = packet->indexInEntity;
          if (indexInEntity == 255) {
            rgbRing.off();
          }
          else {
            TargetInfo info = registry.getTargetInfo();
            if (indexInEntity == info.indexInEntity) {
              Color entityColor = ColorPalette::getByIndex(info.entityColorIndex);
              rgbRing.fill(entityColor.name);
            }
            else {
              rgbRing.off();
            }
          }
        break;
      }

      case OPCODE_TARGET_DISABLE: {
          
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
  //   communication.pairingRequest();
  //   awaitingAck = false;
  //   lastHeartbeat = millis();
  //   heartbeatLost = false;

  //   showAssignedIDBriefly(registry.getAssignedID());
  // }

  if (millis() - lastHeartbeat > 10000 && !heartbeatLost) {
    Serial.println(F("💔 No heartbeat. Waiting to be contacted by Hub..."));                                          // TO ADD AN HEARTBEAT REPLY SO THAT THE HUB CAN KNOW WHO IS STILL CONNECTED
    heartbeatLost = true;
    showStatus(statusRgbLed, STATUS_DISCONNECTED);

    registry.switchToPairingPipe();
    lastHeartbeat = millis();
  }

  // uint8_t targetId = registry.getTargetInfo().id;
  TargetInfo info = registry.getTargetInfo();
  if (info.id != 0xFF) {
    if (sensor.isHit()) {
      if (sessionStatus == GameSessionStatus::Playing && info.enabled == true) {
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