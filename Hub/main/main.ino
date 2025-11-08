#include <DisplayFeedback.h>
#include <Protocol.h>
#include <OPCodes.h>

#include "HubConfig.h"
#include "HubPins.h"
#include "HubStateManager.h"
// communication
#include "WirelessHub.h"
#include "PairingRegistry.h"
#include "Send.h"
#include "Receive.h"
#include "Communication.h"

#include "CommandConsole.h"
#include "LcdDisplay.h"
#include "ScreenManager.h"
#include "ScreenRenderer.h"
#include "ScreenController.h"
#include "GameModeRegistry.h"
#include "ScreenTypes.h"
#include "TargetTypeManager.h"
#include "GameLogic.h"
#include "GameSessionManager.h"

// 🧠 Core Managers
HubStateManager hubState;
WirelessHub wireless;
PairingRegistry registry;
GameModeRegistry gameModeRegistry;
TargetTypeManager targetTypeManager;
LcdDisplay display(lcdI2CAddress, 20, 4);

GameSessionManager sessionManager(registry);

GameLogic gameLogic(sessionManager, registry);

CommandConsole console(registry, send, encoder, leftButton, rightButton, targetTypeManager);

Receive receive(targetTypeManager, registry);
Send send(wireless.getRadio(), registry);
Communication communication(receive, send, registry, gameLogic, statusRgbLed, sessionManager);

ScreenManager screenManager(display, hubState, gameModeRegistry, registry, gameLogic, communication);
ScreenRenderer screenRenderer(display, screenManager, hubState, gameModeRegistry);
ScreenController screenController(screenManager, hubState, registry, gameModeRegistry, encoder, leftButton, rightButton);

// ⏱️ Timing
unsigned long lastHeartbeat = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("🧠 Hub starting..."));
  initializeHubPins();
  wireless.initialize();

  // 🔍 Broadcast pairing poll for 5 seconds after boot
  Serial.println(F("📡 Broadcasting pairing poll..."));
  // unsigned long pairingPollStart = millis();
  // while (millis() - pairingPollStart < 5000) {
    communication.pairingRequest();  // Sends OPCODE_PAIRING_POLL
  //   delay(500);  // Poll every 500ms
  // }

  showStatus(statusRgbLed, STATUS_PAIRING);
  delay(500);
  statusRgbLed.off();

  targetTypeManager.loadFromEEPROM();
  // Serial.print(F("📦 Allowed target type: "));
  // Serial.println(targetTypeToString(targetTypeManager.getAllowedType()));

  display.setup();
  screenManager.setup();
  timeDisplay.setup(4);
}

void loop() {
  screenController.update();
  screenRenderer.render();
  console.processSerial();

  if (statusButton.wasPressed()) {
    // I NEED TO KEEP IN MEMORY WHAT WAS PREVIOUSLY ON THE DISPLAY SO THAT AFTER I CAN RESET IT
    Serial.println(F("statusButton.wasPressed()"));
    send.blinkAll(registry);
    showStatus(statusRgbLed, STATUS_OK, 2);
  }

  if (statusButton.isLongPressed()) {
    Serial.println(F("statusButton.isLongPressed()"));
    registry.clearAll();
    showStatus(statusRgbLed, STATUS_OK, 4);
  }

  if (batteryButton.wasPressed()) {
    Serial.println(F("🔋 Battery check triggered"));
    // showStatus(batteryRgbLed, STATUS_OK);                  // TO FIX BATTERY CHECK
    // batteryRgbLed.pulse("Yellow", 10, 20);
  }

  if (millis() - lastHeartbeat > 3000) {
    send.heartbeatAll(registry);
    lastHeartbeat = millis();
  }

  if (!wireless.available()) return;

  byte buffer[8];
  wireless.read(buffer, sizeof(buffer));
  PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

  Serial.print(F("Request received with opcode: "));
  Serial.println(header->opcode);

  switch (header->opcode) {
    case OPCODE_VERIFICATION_REQUEST: communication.verification(buffer); break;
    case OPCODE_PAIRING_REQUEST:      communication.pairingResponse(buffer); break;
    case OPCODE_HIT_REQUEST: {
        if (sessionManager.getStatus() == GameSessionStatus::Playing) {
          communication.hit(buffer);
        }
      break;
    }
  }
}