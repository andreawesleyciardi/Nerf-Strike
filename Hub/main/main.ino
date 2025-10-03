#include <DisplayFeedback.h>
#include <Protocol.h>
#include "HubConfig.h"
#include "HubPins.h"
#include "HubStateManager.h"
#include "WirelessHub.h"
#include "Send.h"
#include "PairingRegistry.h"
#include "CommandConsole.h"
#include "OPCodes.h"
#include "DisplayManager.h"
#include "ScreenManager.h"
#include "ScreenRenderer.h"
#include "ScreenController.h"
#include "GameModeRegistry.h"
#include "ScreenTypes.h"
#include <TargetType.h>
#include "TargetTypeManager.h"
#include "GameLogic.h"

// 🧠 Core Managers
HubStateManager hubState;
WirelessHub wireless;
Send send(wireless.getRadio());
PairingRegistry registry;
GameModeRegistry gameModeRegistry;
TargetTypeManager targetTypeManager;
DisplayManager display(lcdI2CAddress, 20, 4);
ScreenManager screenManager;
ScreenRenderer screenRenderer(display, screenManager, hubState, gameModeRegistry);
ScreenController screenController(screenManager, hubState, registry, gameModeRegistry, encoder, leftButton, rightButton);
CommandConsole console(registry, send, encoder, leftButton, rightButton, targetTypeManager);

// ⏱️ Timing
unsigned long lastHeartbeat = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("🧠 Hub starting..."));
  initializeHubPins();
  wireless.initialize();

  showStatus(statusRgbLed, STATUS_PAIRING);
  delay(500);
  statusRgbLed.off();

  targetTypeManager.loadFromEEPROM();
  Serial.print(F("📦 Allowed target type: "));
  Serial.println(targetTypeToString(targetTypeManager.getAllowedType()));

  display.setup();
  screenManager.setup();
}

void loop() {
  screenController.update();
  screenRenderer.render();
  console.processSerial();

  if (statusButton.isLongPressed()) {
    send.blinkAll(registry);
    showStatus(statusRgbLed, STATUS_OK, 3);
  }

  if (batteryButton.wasPressed()) {
    Serial.println(F("🔋 Battery check triggered"));
    showStatus(batteryRgbLed, STATUS_OK);
    batteryRgbLed.pulse("Yellow", 10, 20);
  }

  if (leftButton.wasPressed()) {
    Serial.println(F("Left Button pressed"));
    showStatus(statusRgbLed, STATUS_ERROR, 2);
  }

  if (rightButton.wasPressed()) {
    Serial.println(F("Right Button pressed"));
    statusRgbLed.pulse("Purple", 10, 20);
  }

  if (millis() - lastHeartbeat > 3000) {
    send.heartbeatAll(registry);
    lastHeartbeat = millis();
  }

  if (!wireless.available()) return;

  byte buffer[8];
  wireless.read(buffer, sizeof(buffer));
  PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

  switch (header->opcode) {
    case OPCODE_VERIFICATION_REQUEST: {
        VerificationRequestPacket* request = reinterpret_cast<VerificationRequestPacket*>(buffer);

        Serial.print(F("🔍 Verification request for ID: "));
        Serial.println(request->id);
        const uint8_t* pipe = registry.getPipeForID(request->id);
        if (pipe) {
          Serial.println(F("✅ Pipe found. Sending verification ACK."));
          send.verificationResponse(request->id);
        } else {
          Serial.println(F("❌ No pipe found for ID."));
          showStatus(statusRgbLed, STATUS_ERROR);
        }
      break;
    }

    case OPCODE_PAIRING_REQUEST: {
        const PairingRequestPacket* request = reinterpret_cast<const PairingRequestPacket*>(buffer);
        const uint32_t token = request->token;
        const TargetType incomingType = request->type;

        Serial.print(F("🔐 Pairing request received with token: "));
        Serial.println(token);
        Serial.print(F("📦 Incoming target type: "));
        Serial.println(targetTypeToString(incomingType));

        if (!targetTypeManager.isCompatible(incomingType)) {
          Serial.print(F("❌ Target type mismatch. Expected "));
          Serial.print(targetTypeToString(targetTypeManager.getAllowedType()));
          Serial.print(F(", but got "));
          Serial.println(targetTypeToString(incomingType));
          showStatus(statusRgbLed, STATUS_ERROR, 2);
          break;
        }

        const uint8_t assignedID = registry.assignID(token);
        if (assignedID != 0xFF) {
          char pipeName[6];
          sprintf(pipeName, "TGT%d", assignedID);
          registry.storePipeForID(assignedID, reinterpret_cast<uint8_t*>(pipeName));

          Serial.print(F("📡 Stored pipe for ID "));
          Serial.print(assignedID);
          Serial.print(F(": "));
          Serial.println(pipeName);

          send.pairingResponse(assignedID, incomingType);
          showStatus(statusRgbLed, STATUS_PAIRING, 2);
          delay(100);
        } else {
          Serial.println(F("❌ Failed to assign ID."));
          showStatus(statusRgbLed, STATUS_ERROR, 2);
        }
      break;
    }

    case OPCODE_HIT_PACKET: {
        HitPacket* packet = reinterpret_cast<HitPacket*>(buffer);
        uint8_t targetId = packet->id;

        Serial.print(F("🎯 Hit received from target ID: "));
        Serial.println(targetId);

        // 🔢 Update score using game logic
        uint8_t newScore = gameLogic.incrementScoreFor(targetId);

        // 📦 Send updated score back to target
        ScoreUpdatePacket response = {
          OPCODE_SCORE_UPDATE,
          newScore
        };

        const uint8_t* pipe = registry.getPipeForID(targetId);
        if (pipe) {
          send.toTargetPipe(targetId, pipe, reinterpret_cast<const byte*>(&response), sizeof(response));
          Serial.print(F("📤 Score update sent to target ID "));
          Serial.print(targetId);
          Serial.print(F(": "));
          Serial.println(newScore);
        } else {
          Serial.println(F("❌ No pipe found for target ID."));
          showStatus(statusRgbLed, STATUS_ERROR);
        }
      break;
    }
  }
}