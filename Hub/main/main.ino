#include "HubConfig.h"
#include "WirelessHub.h"
#include "PairingRegistry.h"
#include "CommandConsole.h"

WirelessHub wireless;
PairingRegistry registry;
CommandConsole console(registry, wireless);

const int resetPin = 8;
unsigned long pressStart = 0;
unsigned long lastHeartbeat = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("🧠 Hub starting...");
  pinMode(resetPin, INPUT_PULLUP);
  wireless.initialize();
}

void loop() {
  console.processSerial();  // ✅ Enable command handling

  if (digitalRead(resetPin) == LOW) {
    if (pressStart == 0) pressStart = millis();
    if (millis() - pressStart >= 5000) {
      triggerBlinkOnTargets();
      pressStart = 0;
    }
  } else {
    pressStart = 0;
  }

  if (millis() - lastHeartbeat > 3000) {
    sendHeartbeatToTargets();
    lastHeartbeat = millis();
  }

  if (!wireless.available()) return;

  byte buffer[8];
  wireless.read(buffer, sizeof(buffer));

  if (buffer[0] == 0x02) {
    uint8_t id = buffer[1];
    Serial.print("🔍 Verification request for ID: ");
    Serial.println(id);

    const uint8_t* pipe = registry.getPipeForID(id);
    if (pipe) {
      Serial.println("✅ Pipe found. Sending verification ACK.");
      wireless.sendVerificationAck(id);
    } else {
      Serial.println("❌ No pipe found for ID.");
    }
  }

  else if (buffer[0] == 0x01) {
    uint32_t token;
    memcpy(&token, &buffer[1], sizeof(token));
    Serial.print("🔐 Pairing request received with token: ");
    Serial.println(token);

    uint8_t assignedID = registry.assignID(token);
    if (assignedID != 0xFF) {
      char pipeName[6];
      sprintf(pipeName, "TGT%d", assignedID);
      registry.storePipeForID(assignedID, (uint8_t*)pipeName);
      Serial.print("📡 Stored pipe for ID ");
      Serial.print(assignedID);
      Serial.print(": ");
      Serial.println(pipeName);

      wireless.sendPairingResponse(assignedID);
      delay(100);
    } else {
      Serial.println("❌ Failed to assign ID.");
    }
  }
}

void triggerBlinkOnTargets() {
  Serial.println("🔦 Triggering blink on all paired targets...");
  byte msg[2] = {0x05, 0x00};
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      Serial.print("🔦 Sending blink to ID: ");
      Serial.println(id);
      wireless.sendToTargetPipe(id, pipe, msg, sizeof(msg));
    }
  }
}

void sendHeartbeatToTargets() {
  byte ping[2] = {0x04, 0x00};
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      wireless.sendToTargetPipe(id, pipe, ping, sizeof(ping));
    }
  }
}
