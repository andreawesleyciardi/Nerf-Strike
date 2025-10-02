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

// 🧠 Core Managers
HubStateManager hubState;
WirelessHub wireless;
PairingRegistry registry;
GameModeRegistry gameModeRegistry;
DisplayManager display(lcdI2CAddress, 20, 4);
ScreenManager screenManager;
ScreenRenderer screenRenderer(display, screenManager, hubState, gameModeRegistry);
ScreenController screenController(screenManager, hubState, registry, gameModeRegistry, encoder, leftButton, rightButton);
CommandConsole console(registry, wireless, encoder, leftButton, rightButton);

// ⏱️ Timing
unsigned long lastHeartbeat = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("🧠 Hub starting..."));
  initializeHubPins();
  wireless.initialize();

  showStatus(statusRgbLed, STATUS_PAIRING);  // Startup indicator
  delay(500);
  statusRgbLed.off();

  display.setup();
  screenManager.setup();
  // registry.setup();
  // hubState.setup();
  // console.setup();
}

void loop() {
  // encoder.update();
  // leftButton.update();
  // rightButton.update();

  // 🖥️ 20x4 LCD (I2C)
  screenController.update();  // 🧭 Screen-aware input routing
  screenRenderer.render();    // 🎨 Draw current screen

  // 🧵 Serial Command Console
  console.processSerial();

  // 🔘 Long press on status button triggers blink
  if (statusButton.isLongPressed()) {
    triggerBlinkOnTargets();
    showStatus(statusRgbLed, STATUS_OK, 3);
  }

  // 🔋 Battery button triggers battery LED pulse
  if (batteryButton.wasPressed()) {
    Serial.println(F("🔋 Battery check triggered"));
    showStatus(batteryRgbLed, STATUS_OK);
    batteryRgbLed.pulse("Yellow", 10, 20);
  }

  // 🧭 Navigation buttons (global feedback only)
  if (leftButton.wasPressed()) {
    Serial.println(F("↩️ Cancel pressed"));
    showStatus(statusRgbLed, STATUS_ERROR, 2);
  }

  if (rightButton.wasLongPressed()) {
    Serial.println(F("✅ Save long-pressed"));
    statusRgbLed.pulse("Purple", 10, 20);
  }

  // 💓 Heartbeat to targets
  if (millis() - lastHeartbeat > 3000) {
    sendHeartbeatToTargets();
    lastHeartbeat = millis();
  }

  // 📡 Incoming wireless packet
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
    Serial.print(F("🔐 Pairing request received with token: "));
    Serial.println(token);

    uint8_t assignedID = registry.assignID(token);
    if (assignedID != 0xFF) {
      char pipeName[6];
      sprintf(pipeName, "TGT%d", assignedID);
      registry.storePipeForID(assignedID, (uint8_t*)pipeName);
      Serial.print(F("📡 Stored pipe for ID "));
      Serial.print(assignedID);
      Serial.print(F(": "));
      Serial.println(pipeName);

      wireless.sendPairingResponse(assignedID);
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