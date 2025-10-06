#include <DisplayFeedback.h>
#include <Protocol.h>
#include "HubConfig.h"
#include "HubPins.h"
#include "HubStateManager.h"
#include "WirelessHub.h"
#include "Send.h"
#include "Receive.h"
#include "Communication.h"
#include "PairingRegistry.h"
#include "CommandConsole.h"
#include "OPCodes.h"
#include "LcdDisplay.h"
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
LcdDisplay display(lcdI2CAddress, 20, 4);
ScreenManager screenManager;
ScreenRenderer screenRenderer(display, screenManager, hubState, gameModeRegistry);
ScreenController screenController(screenManager, hubState, registry, gameModeRegistry, encoder, leftButton, rightButton);
CommandConsole console(registry, send, encoder, leftButton, rightButton, targetTypeManager);

Receive receive(targetTypeManager, registry);
Send send(wireless.getRadio());

Communication communication(receive, send, registry, gameLogic, statusRgbLed);

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
    case OPCODE_VERIFICATION_REQUEST: communication.verification(buffer); break;
    case OPCODE_PAIRING_REQUEST:      communication.pairing(buffer); break;
    case OPCODE_HIT_EVENT:            communication.hit(buffer); break;
  }
}