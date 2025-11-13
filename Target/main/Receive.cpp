#include "Receive.h"
#include <Protocol.h>
#include <OPCodes.h>
#include <Score.h>
#include "TargetConfig.h"

Receive::Receive(RF24& radio, PairingRegistry& registry)
  : radio(radio), registry(registry) {}

TargetInfo Receive::pairingResponse(uint32_t token) {
  radio.openReadingPipe(1, pairingPipe);
  radio.startListening();

  Serial.print(F("📡 Target waiting for pairing response on pairing pipe: 0x"));
  Serial.print((uint32_t)(pairingPipe >> 32), HEX);  // High 32 bits
  Serial.println((uint32_t)(pairingPipe & 0xFFFFFFFF), HEX);  // Low 32 bits

  TargetInfo target;

  unsigned long startTime = millis();

  bool hasTimedOut = true;
  while (millis() - startTime < 6000) {
    if (radio.available()) {
      hasTimedOut = false;
      PairingResponsePacket response;
      radio.read(&response, sizeof(response));

      Serial.print(F("⏳ Receiving pairing response with opcode: "));
      Serial.println(response.opcode);

      Serial.println();
      if (response.opcode == OPCODE_PAIRING_RESPONSE && response.token == token) {
        target.id = response.assignedID;
        target.colorIndex = response.colorIndex;
        target.token = token;

        Serial.println(F("✅ Received pairing response."));
        Serial.print(F("🆔 Assigned ID: "));
        Serial.println(response.assignedID);
        Serial.print(F("🎨 Assigned Color Index: "));
        Serial.println(target.colorIndex);
        break;
      }
      else {
        Serial.println(F("❌ Received wrong credentials for Pairing Response."));
        Serial.print(F("📜 Received OPCODE: "));
        Serial.println(response.opcode);
        Serial.print(F("📜 Expected OPCODE: "));
        Serial.println(OPCODE_PAIRING_RESPONSE);
        Serial.print(F("🧾 Received token: "));
        Serial.println(response.token);
        Serial.print(F("🧾 Expected token: "));
        Serial.println(token);
      }
    }
  }

  radio.stopListening();

  if (!target.isValid()) {
    Serial.println(F("❌ No valid pairing response received."));
    if (hasTimedOut) {
      Serial.println(F("❌ Cause: Time Out."));
    }
  }

  return target;
}

const bool Receive::verificationResponse(uint8_t id) {
  Serial.println(F("⏳ Waiting for verification acknowledgment..."));
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    if (radio.available()) {                                                        // MOVE TO main.ino with others opcodes
      VerificationResponsePacket response;
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

HitResponsePacket Receive::hitResponse() {
  Serial.println(F("⏳ Waiting for hit acknowledgment..."));
  HitResponsePacket response = {
    OPCODE_SCORE_UPDATE,
    0xFF,
    ScoreStatus::Waiting
  };

  unsigned long startTime = millis();
  while (millis() - startTime < 100) {
    if (radio.available()) {
      byte buffer[8];
      radio.read(buffer, sizeof(buffer));

      PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
      if (header->opcode == OPCODE_SCORE_UPDATE) {
        response = *reinterpret_cast<HitResponsePacket*>(buffer);
        Serial.print(F("✅ Hit acknowledged. Updated score: "));
        Serial.println(response.value);
        break;
      }
    }
  }

  return response;
}

const bool Receive::identifyTargetRequest(const byte* buffer) {
  const IdentifyTargetRequestPacket* request = reinterpret_cast<const IdentifyTargetRequestPacket*>(buffer);
  return request->switchOn;
}