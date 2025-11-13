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

Receive receive(targetTypeManager, registry);
Send send(wireless.getRadio(), registry);
Communication communication(receive, send, registry, gameLogic, statusRgbLed, sessionManager);

ScreenManager screenManager(display, hubState, gameModeRegistry, registry, gameLogic, communication);
ScreenRenderer screenRenderer(display, screenManager, hubState, gameModeRegistry);
ScreenController screenController(screenManager, hubState, registry, gameModeRegistry, encoder, leftButton, rightButton);

CommandConsole console(registry, send, encoder, leftButton, rightButton, targetTypeManager, communication);

// ⏱️ Timing
unsigned long lastHeartbeat = 0;

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("🚀🚀🚀🚀🚀🚀🚀"));
  Serial.println(F("🧠 Hub starting..."));

  // 🛠️ Initialize hardware and wireless
  initializeHubPins();
  wireless.initialize();

  targetTypeManager.loadFromEEPROM();
  // Serial.print(F("📦 Allowed target type: "));
  // Serial.println(targetTypeToString(targetTypeManager.getAllowedType()));

  // 🔍 Broadcast pairing poll for 5 seconds after boot
  Serial.println();
  Serial.println(F("📡 Broadcasting pairing poll..."));
  // unsigned long pairingPollStart = millis();
  // while (millis() - pairingPollStart < 5000) {
    if (communication.pairingSollecitation()){                                                   // Sends OPCODE_PAIRING_SOLLECITATION
      showStatus(statusRgbLed, STATUS_PAIRING);
    }
    else {
      showStatus(statusRgbLed, STATUS_ERROR);
    }
  //   delay(500);  // Poll every 500ms
  // }
  delay(500);
  statusRgbLed.off();

  display.setup();
  screenManager.setup();
  timeDisplay.setup(4);

  Serial.println(F("✅ Hub setup complete."));
}
// void setup() {
//   Serial.begin(9600);
//   Serial.println(F("🧠 Hub starting..."));
//   initializeHubPins();
//   wireless.initialize();

//   // 🔍 Broadcast pairing poll for 5 seconds after boot
//   unsigned long pairingPollStart = millis();
//   while (millis() - pairingPollStart < 5000) {
//     communication.pairingRequest();
//     delay(500);  // Poll every 500ms
//   }

//   showStatus(statusRgbLed, STATUS_PAIRING);
//   delay(500);
//   statusRgbLed.off();

//   targetTypeManager.loadFromEEPROM();
//   Serial.print(F("📦 Allowed target type: "));
//   Serial.println(targetTypeToString(targetTypeManager.getAllowedType()));

//   display.setup();
//   screenManager.setup();
//   timeDisplay.setup(4);
// }

void loop() {
  screenController.update();
  screenRenderer.render();
  console.processSerial();

  if (statusButton.wasPressed()) {
    // I NEED TO KEEP IN MEMORY WHAT WAS PREVIOUSLY ON THE DISPLAY SO THAT AFTER I CAN RESET IT
    Serial.println();
    Serial.println(F("👆 Status button was pressed."));
    communication.blinkAll();
    Serial.println(F("✨ Sent blink request to all targets."));
    showStatus(statusRgbLed, STATUS_OK, 2);
  }

  if (statusButton.isLongPressed()) {
    Serial.println();
    Serial.println(F("👆 Status button was long pressed."));
    registry.clearAll();
    Serial.println(F("🧰 Registry was completely cleared."));
    showStatus(statusRgbLed, STATUS_OK, 4);
  }

  if (batteryButton.wasPressed()) {
    Serial.println();
    Serial.println(F("👆 Battery button was pressed."));
    // showStatus(batteryRgbLed, STATUS_OK);                  // TO FIX BATTERY CHECK
    // batteryRgbLed.pulse("Yellow", 10, 20);
  }

  if (millis() - lastHeartbeat > 3000) {
    communication.heartbeatAll();
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

    case OPCODE_PAIRING_REQUEST: {
        Serial.println();
        if (communication.pairingResponse(buffer)) {
          showStatus(statusRgbLed, STATUS_CONNECTED, 2);
        }
        else {
          showStatus(statusRgbLed, STATUS_ERROR, 2);
        }
      break;
    }

    case OPCODE_HEARTBEAT_RESPONSE: {
      HeartbeatResponsePacket* packet = reinterpret_cast<HeartbeatResponsePacket*>(buffer);
      uint8_t targetId = packet->id;

      TargetInfo& info = registry.getTargetRefByID(targetId);
      if (info.isValid()) {
        unsigned long now = millis();
        const unsigned long TIMEOUT = 10000; // 10 seconds

        if (info.inactiveFrom != 0) {
          unsigned long inactiveDuration = now - info.inactiveFrom;

          if (inactiveDuration > TIMEOUT) {
            Serial.print(F("❌ Target "));
            Serial.print(targetId);
            Serial.println(F(" has been inactive for too long. Removing..."));
            registry.removeTargetById(targetId);
          } else {
            Serial.print(F("✅ Target "));
            Serial.print(targetId);
            Serial.println(F(" responded again. Clearing inactivity."));
            info.inactiveFrom = 0;
          }
        } else {
          // Target is active, no previous inactivity
          Serial.print(F("🎯 Target "));
          Serial.print(targetId);
          Serial.println(F(" responded to heartbeat."));
        }
      } else {
        Serial.print(F("⚠️ Received heartbeat response from unknown or invalid target ID: "));
        Serial.println(targetId);
      }

      break;
    }

    case OPCODE_HIT_REQUEST: {
        if (sessionManager.getStatus() == GameSessionStatus::Playing) {
          communication.hit(buffer);
        }
      break;
    }
  }
}