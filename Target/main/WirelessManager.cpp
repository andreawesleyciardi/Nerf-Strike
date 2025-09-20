#include "WirelessManager.h"

#define CE_PIN 9
#define CSN_PIN 10

WirelessManager::WirelessManager() : radio(CE_PIN, CSN_PIN) {}

void WirelessManager::setup() {
  Serial.println("🔧 Initializing radio...");

  if (!radio.begin()) {
    Serial.println("❌ Radio failed to initialize. Check wiring and power.");
    return;
  }

  if (!radio.isChipConnected()) {
    Serial.println("❌ Radio chip not connected. Check CE/CSN and SPI pins.");
    return;
  }

  Serial.println("✅ Radio initialized and chip connected.");

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(100);
  radio.openReadingPipe(1, pairingPipe);
  radio.startListening();

  Serial.println("📡 Radio listening on pairing pipe.");
}

struct HitPacket {
  uint8_t targetId;
  char type[4]; // "HIT"
};

bool WirelessManager::sendHitPacket(uint8_t targetId, uint8_t &updatedScore) {
  HitPacket packet = {targetId, "HIT"};

  radio.stopListening();
  bool success = radio.write(&packet, sizeof(packet));

  if (success) {
    radio.startListening();
    unsigned long startTime = millis();
    while (!radio.available()) {
      if (millis() - startTime > 200) {
        Serial.println("⚠️ No response from hub.");
        return false;
      }
    }

    radio.read(&updatedScore, sizeof(updatedScore));
    Serial.print("🎯 Updated score: ");
    Serial.println(updatedScore);
    return true;
  } else {
    Serial.println("❌ Failed to send hit packet.");
    return false;
  }
}

void WirelessManager::sendPairingRequest(uint32_t token) {
  Serial.print("📨 Sending pairing request with token: ");
  Serial.println(token);

  radio.stopListening();
  byte packet[5];
  packet[0] = 1; // Type 0x01 = pairing request
  memcpy(&packet[1], &token, sizeof(token));

  radio.openWritingPipe(pairingPipe);
  bool success = radio.write(&packet, sizeof(packet));
  radio.startListening();

  if (success) {
    Serial.println("✅ Pairing request sent.");
  } else {
    Serial.println("❌ Failed to send pairing request.");
  }
}

bool WirelessManager::receivePairingResponse(uint8_t &assignedID) {
  Serial.println("⏳ Waiting for pairing response...");
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    if (radio.available()) {
      byte response[1];
      radio.read(&response, sizeof(response));
      assignedID = response[0];
      Serial.print("✅ Received pairing response. Assigned ID: ");
      Serial.println(assignedID);
      return true;
    }
  }
  Serial.println("❌ No pairing response received.");
  return false;
}

void WirelessManager::sendVerificationRequest(uint8_t id) {
  Serial.print("📨 Sending verification request for ID ");
  Serial.println(id);

  radio.stopListening();
  byte packet[2] = {0x02, id};
  radio.openWritingPipe(pairingPipe);
  bool success = radio.write(&packet, sizeof(packet));
  radio.startListening();

  if (success) {
    Serial.println("✅ Verification request sent.");
  } else {
    Serial.println("❌ Failed to send verification request.");
  }
}

bool WirelessManager::waitForVerificationAck(uint8_t id) {
  Serial.println("⏳ Waiting for verification acknowledgment...");
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    if (radio.available()) {
      byte response[2];
      radio.read(&response, sizeof(response));
      if (response[0] == 0x03 && response[1] == id) {
        Serial.println("✅ Verification acknowledged by hub.");
        return true;
      } else {
        Serial.println("⚠️ Received unexpected response.");
      }
    }
  }
  Serial.println("❌ No verification acknowledgment received.");
  return false;
}

void WirelessManager::switchToTargetPipe(uint8_t id) {
  sprintf((char*)targetPipe, "TGT%d", id);
  radio.openReadingPipe(1, targetPipe);
  Serial.print("🔀 Switched to target pipe: ");
  Serial.println((char*)targetPipe);
}

void WirelessManager::sendToHub(const byte* data, uint8_t length) {
  radio.stopListening();
  radio.openWritingPipe(targetPipe);
  bool success = radio.write(data, length);
  radio.startListening();

  if (success) {
    Serial.println("📤 Sent data to hub via target pipe.");
  } else {
    Serial.println("❌ Failed to send data to hub.");
  }
}

bool WirelessManager::available() {
  return radio.available();
}

void WirelessManager::read(byte* buffer, uint8_t length) {
  radio.read(buffer, length);
}
