#include "WirelessManager.h"
#include <string.h>

#define CE_PIN 9
#define CSN_PIN 10

WirelessManager::WirelessManager() : radio(CE_PIN, CSN_PIN) {}

void WirelessManager::setup() {
  Serial.println(F("🔧 Initializing radio..."));

  if (!radio.begin()) {
    Serial.println(F("❌ Radio failed to initialize. Check wiring and power."));
    return;
  }

  if (!radio.isChipConnected()) {
    Serial.println(F("❌ Radio chip not connected. Check CE/CSN and SPI pins."));
    return;
  }

  Serial.println(F("✅ Radio initialized and chip connected."));

  radio.setAutoAck(true);         // ✅ Enable auto-ack
  radio.enableAckPayload();       // ✅ Optional: allow ACK payloads
  radio.setRetries(5, 15);        // ✅ Retry settings

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(100);

  radio.openReadingPipe(1, pairingPipe);
  radio.startListening();

  Serial.println(F("📡 Radio listening on pairing pipe."));
}

struct HitPacket {
  uint8_t targetId;
  char type[4]; // "HIT"
};

bool WirelessManager::sendHitPacket(uint8_t targetId) {
  HitPacket packet = {targetId, "HIT"};

  radio.stopListening();
  bool success = radio.write(&packet, sizeof(packet));
  radio.startListening();

  if (success) {
    Serial.println(F("📡 Hit packet sent successfully."));
    return true;
  } else {
    Serial.println(F("❌ Failed to send hit packet."));
    return false;
  }
}

void WirelessManager::sendPairingRequest(uint32_t token) {
  Serial.print(F("📨 Sending pairing request with token: "));
  Serial.println(token);

  radio.stopListening();
  byte packet[5];
  packet[0] = 1;
  memcpy(&packet[1], &token, sizeof(token));

  radio.openWritingPipe(pairingPipe);
  bool success = radio.write(packet, sizeof(packet));
  radio.startListening();

  if (success) {
    Serial.println(F("✅ Pairing request sent."));
  } else {
    Serial.println(F("❌ Failed to send pairing request."));
  }
}

bool WirelessManager::receivePairingResponse(uint8_t &assignedID) {
  radio.openReadingPipe(1, pairingPipe);
  radio.startListening();

  Serial.println(F("⏳ Waiting for pairing response..."));
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    if (radio.available()) {
      byte response[2];
      radio.read(&response, sizeof(response));
      if (response[0] == OPCODE_PAIRING_REQUEST) {
        assignedID = response[1];
        Serial.print(F("✅ Received pairing response. Assigned ID: "));
        Serial.println(assignedID);
        return true;
      }
    }
  }

  Serial.println(F("❌ No pairing response received."));
  return false;
}

void WirelessManager::sendVerificationRequest(uint8_t id) {
  Serial.print(F("📨 Sending verification request for ID "));
  Serial.println(id);

  radio.stopListening();
  byte packet[2] = {OPCODE_VERIFICATION_REQUEST, id};
  radio.openWritingPipe(pairingPipe);
  bool success = radio.write(packet, sizeof(packet));
  radio.startListening();

  if (success) {
    Serial.println(F("✅ Verification request sent."));
  } else {
    Serial.println(F("❌ Failed to send verification request."));
  }
}

bool WirelessManager::waitForVerificationAck(uint8_t id) {
  Serial.println(F("⏳ Waiting for verification acknowledgment..."));
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    if (radio.available()) {
      byte response[2];
      radio.read(&response, sizeof(response));
      if (response[0] == OPCODE_VERIFICATION_ACK && response[1] == id) {
        Serial.println(F("✅ Verification acknowledged by hub."));
        return true;
      }
    }
  }
  Serial.println(F("❌ No verification acknowledgment received."));
  return false;
}

void WirelessManager::switchToTargetPipe(uint8_t id) {
  sprintf((char*)targetPipe, "TGT%d", id);
  radio.stopListening();  // ✅ Stop before switching
  radio.openReadingPipe(1, targetPipe);
  radio.startListening(); // ✅ Start after switching

  Serial.print(F("🔀 Switched to target pipe: "));
  Serial.println((char*)targetPipe);
}

void WirelessManager::sendToHub(const byte* data, uint8_t length) {
  radio.stopListening();
  radio.openWritingPipe(targetPipe);
  bool success = radio.write(data, length);
  radio.startListening();

  if (success) {
    Serial.println(F("📤 Sent data to hub via target pipe."));
  } else {
    Serial.println(F("❌ Failed to send data to hub."));
  }
}

bool WirelessManager::available() {
  return radio.available();
}

void WirelessManager::read(byte* buffer, uint8_t length) {
  radio.read(buffer, length);
}
