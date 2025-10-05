#include <DisplayFeedback.h>
#include <Protocol.h>
#include "HubConfig.h"
#include "HubPins.h"
#include "HubStateManager.h"
#include "WirelessHub.h"
#include "Send.h"
#include "Receive.h"
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
PairingRegistry registry;
GameModeRegistry gameModeRegistry;
TargetTypeManager targetTypeManager;
DisplayManager display(lcdI2CAddress, 20, 4);
ScreenManager screenManager;
ScreenRenderer screenRenderer(display, screenManager, hubState, gameModeRegistry);
ScreenController screenController(screenManager, hubState, registry, gameModeRegistry, encoder, leftButton, rightButton);
CommandConsole console(registry, send, encoder, leftButton, rightButton, targetTypeManager);

Send send(wireless.getRadio());
Receive receive(targetTypeManager, registry);

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
        const uint8_t* pipe = receive.verificationRequest(buffer);
        if (!pipe) {
          Serial.println(F("❌ No pipe found for ID."));
          showStatus(statusRgbLed, STATUS_ERROR);
        } else {
          uint8_t targetId = reinterpret_cast<VerificationRequestPacket*>(buffer)->id;
          Serial.println(F("✅ Pipe found. Sending verification ACK."));
          send.verificationResponse(targetId);
        }
      break;
    }

    case OPCODE_PAIRING_REQUEST: {
        const uint8_t assignedID = receive.pairingRequest(buffer);
        if (assignedID != 0xFF) {
          TargetType incomingType = reinterpret_cast<PairingRequestPacket*>(buffer)->type;
          send.pairingResponse(assignedID, incomingType);

          char pipeName[6];
          sprintf(pipeName, "TGT%d", assignedID);
          registry.storePipeForID(assignedID, reinterpret_cast<uint8_t*>(pipeName));

          Serial.print(F("📡 Stored pipe for ID "));
          Serial.print(assignedID);
          Serial.print(F(": "));
          Serial.println(pipeName);
          
          showStatus(statusRgbLed, STATUS_PAIRING, 2);
          delay(100);
        } else {
          Serial.println(F("❌ Failed to assign ID."));
          showStatus(statusRgbLed, STATUS_ERROR, 2);
        }
      break;
    }

    case OPCODE_HIT_PACKET: {
        const uint8_t* pipe = receive.hitPacket(buffer);
        if (!pipe) {
          Serial.println(F("❌ No pipe found for target ID."));
          showStatus(statusRgbLed, STATUS_ERROR);
        } else {
          uint8_t targetId = reinterpret_cast<HitPacket*>(buffer)->id;
          uint8_t newScore = gameLogic.incrementScoreFor(targetId);
          send.scoreUpdate(targetId, pipe, newScore);
        }
      break;
    }
  }
}