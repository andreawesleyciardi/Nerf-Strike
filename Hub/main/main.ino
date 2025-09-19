#include "HubConfig.h"
#include "WirelessHub.h"
#include "PairingRegistry.h"
#include "CommandConsole.h"

WirelessHub wireless;
PairingRegistry registry;
CommandConsole console(registry, wireless);

const int resetPin = 8; // physical reset button
unsigned long pressStart = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("🧠 Hub starting...");

  pinMode(resetPin, INPUT_PULLUP);
  wireless.initialize();
}

void loop() {
  console.processSerial();

  // Detect long-press on reset button
  if (digitalRead(resetPin) == LOW) {
    if (pressStart == 0) pressStart = millis();
    if (millis() - pressStart >= 5000) {
      triggerBlinkOnTargets();
      pressStart = 0;
    }
  } else {
    pressStart = 0;
  }

  if (!wireless.available()) return;

  byte buffer[8];
  wireless.read(buffer, sizeof(buffer));

  if (buffer[0] == 0x02) {
    uint8_t id = buffer[1];
    const uint8_t* pipe = registry.getPipeForID(id);
    if (pipe) {
      wireless.sendVerificationAck(id);
    }
  } else if (buffer[0] == 0x01) {
    uint32_t token;
    memcpy(&token, &buffer[1], sizeof(token));
    uint8_t assignedID = registry.assignID(token);
    if (assignedID != 0xFF) {
      wireless.sendPairingResponse(assignedID);
    }
  }
}

void triggerBlinkOnTargets() {
  Serial.println("🔦 Triggering blink on all paired targets...");
  byte msg[2] = {0x05, 0x00}; // Type 0x05 = blink command
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      wireless.sendToTargetPipe(id, pipe, msg, sizeof(msg));
    }
  }
}
