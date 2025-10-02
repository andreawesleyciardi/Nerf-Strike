#include <DisplayFeedback.h>
#include "HubConfig.h"
#include "HubPins.h"
#include "HubStateManager.h"
#include "WirelessHub.h"
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
CommandConsole console(registry, wireless, encoder, leftButton, rightButton, targetTypeManager);

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
    triggerBlinkOnTargets();
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
    sendHeartbeatToTargets();
    lastHeartbeat = millis();
  }

  if (!wireless.available()) return;

  byte buffer[8];
  wireless.read(buffer, sizeof(buffer));

  if (buffer[0] == OPCODE_VERIFICATION_REQUEST) {
    uint8_t id = buffer[1];
    Serial.print(F("🔍 Verification request for ID: "));
    Serial.println(id);

    const uint8_t* pipe = registry.getPipeForID(id);
    if (pipe) {
      Serial.println(F("✅ Pipe found. Sending verification ACK."));
      wireless.sendVerificationAck(id);
    } else {
      Serial.println(F("❌ No pipe found for ID."));
      showStatus(statusRgbLed, STATUS_ERROR);
    }
  }

  else if (buffer[0] == OPCODE_PAIRING_REQUEST) {
    uint32_t token;
    memcpy(&token, &buffer[1], sizeof(token));
    TargetType incomingType = static_cast<TargetType>(buffer[5]);

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
      return;
    }

    uint8_t assignedID = registry.assignID(token);
    if (assignedID != 0xFF) {
      char pipeName[6];
      sprintf(pipeName, "TGT%d", assignedID);
      registry.storePipeForID(assignedID, (uint8_t*)pipeName);
      Serial.print(F("📡 Stored pipe for ID "));
      Serial.print(assignedID);
      Serial.print(F(": "));
      Serial.println(pipeName);

      wireless.sendPairingResponse(assignedID, TargetType::StrikeLite);
      showStatus(statusRgbLed, STATUS_PAIRING, 2);
      delay(100);
    } else {
      Serial.println(F("❌ Failed to assign ID."));
      showStatus(statusRgbLed, STATUS_ERROR, 2);
    }
  }
}

void triggerBlinkOnTargets() {
  Serial.println(F("🔦 Triggering blink on all paired targets..."));
  byte msg[2] = {OPCODE_BLINK_COMMAND, 0x00};
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      Serial.print(F("🔦 Sending blink to ID: "));
      Serial.println(id);
      wireless.sendToTargetPipe(id, pipe, msg, sizeof(msg));
    }
  }
}

void sendHeartbeatToTargets() {
  byte ping[2] = {OPCODE_HEARTBEAT, 0x00};
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      wireless.sendToTargetPipe(id, pipe, ping, sizeof(ping));
    }
  }
}