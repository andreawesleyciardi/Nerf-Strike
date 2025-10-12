#include "Receive.h"
#include <Protocol.h>
#include <OPCodes.h>
#include "TargetConfig.h"

Receive::Receive(RF24& radio, PairingRegistry& registry)
  : radio(radio), registry(registry) {}

const uint8_t Receive::pairingResponse() {
  Serial.print(F("📡 Target waiting for pairing response on pairing pipe: 0x"));
  Serial.print((uint32_t)(pairingPipe >> 32), HEX);  // High 32 bits
  Serial.println((uint32_t)(pairingPipe & 0xFFFFFFFF), HEX);  // Low 32 bits

  radio.openReadingPipe(1, pairingPipe);
  radio.startListening();

  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    if (radio.available()) {
      PairingResponsePacket response;
      radio.read(&response, sizeof(response));
      Serial.print(F("⏳ Receiving pairing response with opcode: "));
      Serial.println(response.opcode);
      if (response.opcode == OPCODE_PAIRING_RESPONSE) {
        Serial.print(F("✅ Received pairing response. Assigned ID: "));
        Serial.println(response.assignedID);
        return response.assignedID;
      }
    }
  }

  Serial.println(F("❌ No pairing response received."));
  return 0xFF;
}

const bool Receive::verificationResponse(uint8_t id) {
  Serial.println(F("⏳ Waiting for verification acknowledgment..."));
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    if (radio.available()) {
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
    0xFF 
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
        Serial.println(response.score);
        break;
      }
    }
  }

  return response;
}

const String Receive::entityColor(const byte* buffer) {
  EntityColorRequestPacket* request = reinterpret_cast<EntityColorRequestPacket*>(const_cast<byte*>(buffer));
  return request->name;
}