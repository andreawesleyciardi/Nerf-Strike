#include "WirelessManager.h"
#include <TargetType.h>
#include <OPCodes.h>
#include <Protocol.h>
#include "TargetConfig.h"

WirelessManager::WirelessManager() : radio(transmitterCEPin, transmitterCSNPin) {}

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

  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.setRetries(5, 15);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(100);

  radio.openReadingPipe(1, pairingPipe);
  radio.startListening();

  Serial.println(F("📡 Radio listening on pairing pipe."));
}

void WirelessManager::sendPairingRequest(uint32_t token) {
  PairingRequest request = {
    OPCODE_PAIRING_REQUEST,
    token,
    targetType  // from TargetConfig.h
  };

  Serial.print(F("📨 Sending pairing request with token: "));
  Serial.println(token);

  radio.stopListening();
  radio.openWritingPipe(pairingPipe);
  bool success = radio.write(&request, sizeof(request));
  radio.startListening();

  if (success) {
    Serial.println(F("✅ Pairing request sent."));
  } else {
    Serial.println(F("❌ Failed to send pairing request."));
  }
}

bool WirelessManager::receivePairingResponse(uint8_t &assignedID) {
  TargetType dummyType;
  return receivePairingResponse(assignedID, dummyType);
}

bool WirelessManager::receivePairingResponse(uint8_t &assignedID, TargetType &typeOut) {
  radio.openReadingPipe(1, pairingPipe);
  radio.startListening();

  Serial.println(F("⏳ Waiting for pairing response..."));
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    if (radio.available()) {
      PairingResponse response;
      radio.read(&response, sizeof(response));

      if (response.opcode == OPCODE_PAIRING_RESPONSE) {
        assignedID = response.assignedID;
        typeOut = response.type;

        Serial.print(F("✅ Received pairing response. Assigned ID: "));
        Serial.print(assignedID);
        Serial.print(F(" | Type: "));
        Serial.println(targetTypeToString(typeOut));
        return true;
      }
    }
  }

  Serial.println(F("❌ No pairing response received."));
  return false;
}

void WirelessManager::sendVerificationRequest(uint8_t id) {
  VerificationRequest request = {
    OPCODE_VERIFICATION_REQUEST,
    id
  };

  Serial.print(F("📨 Sending verification request for ID "));
  Serial.println(id);

  radio.stopListening();
  radio.openWritingPipe(pairingPipe);
  bool success = radio.write(&request, sizeof(request));
  radio.startListening();

  if (success) {
    Serial.println(F("✅ Verification request sent."));
  } else {
    Serial.println(F("❌ Failed to send verification request."));
  }
}

bool WirelessManager::waitForVerificationResponse(uint8_t id) {
  Serial.println(F("⏳ Waiting for verification acknowledgment..."));
  unsigned long startTime = millis();

  while (millis() - startTime < 1000) {
    if (radio.available()) {
      VerificationResponse response;
      radio.read(&response, sizeof(response));

      if (response.opcode == OPCODE_VERIFICATION_RESPONSE && response.id == id) {
        Serial.print(F("✅ Verification acknowledged by hub. ID: "));
        Serial.println(response.id);
        return true;
      }
    }
  }

  Serial.println(F("❌ No verification acknowledgment received."));
  return false;
}

bool WirelessManager::sendHitPacket(uint8_t targetId) {
  HitPacket packet = {
    targetId,
    "HIT"
  };

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

// Utilities functions ---------------------------------------------------------------------------------

void WirelessManager::switchToTargetPipe(uint8_t id) {
  sprintf((char*)targetPipe, "TGT%d", id);
  radio.stopListening();
  radio.openReadingPipe(1, targetPipe);
  radio.startListening();

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