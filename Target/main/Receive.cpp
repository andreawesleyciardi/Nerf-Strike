#include "Receive.h"
#include <Protocol.h>
#include <OPCodes.h>
#include <Score.h>
#include "TargetConfig.h"

Receive::Receive(RF24& radio, PairingRegistry& registry)
  : radio(radio), registry(registry) {}

TargetInfo Receive::pairingResponse() {
  Serial.print(F("📡 Target waiting for pairing response on pairing pipe: 0x"));
  Serial.print((uint32_t)(pairingPipe >> 32), HEX);  // High 32 bits
  Serial.println((uint32_t)(pairingPipe & 0xFFFFFFFF), HEX);  // Low 32 bits

  radio.openReadingPipe(1, pairingPipe);
  radio.startListening();

  TargetInfo target;  // Default-initialized (invalid)

  unsigned long startTime = millis();

                                                                                                  // THE PROBLEM IS HERE...looks that radio is never available...
  while (millis() - startTime < 6000) {                                                           // Maybe the time is too short
    if (radio.available()) {
      PairingResponsePacket response;
      radio.read(&response, sizeof(response));

      Serial.print(F("⏳ Receiving pairing response with opcode: "));
      Serial.println(response.opcode);

      if (response.opcode == OPCODE_PAIRING_RESPONSE) {
        target.id = response.assignedID;
        target.colorIndex = response.colorIndex;

        Serial.print(F("✅ Received pairing response."));
        Serial.print(F("🆔 Assigned ID: "));
        Serial.println(response.assignedID);
        Serial.print(F("🎨 Assigned Color Index: "));
        Serial.println(target.colorIndex);
        break;
      }
      else {
        Serial.print(F("❌ Received wrong opcode for Pairing Response. Expected "));
        Serial.println(OPCODE_PAIRING_RESPONSE);
      }
    }
  }

  radio.stopListening();

  if (!target.isValid()) {
    Serial.println(F("❌ No valid pairing response received."));
  }

  return target;
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

// const String Receive::entityColor(const byte* buffer) {
//   const EntityColorRequestPacket* request = reinterpret_cast<const EntityColorRequestPacket*>(buffer);

//   Serial.print(F("📦 Received opcode: "));
//   Serial.println(request->opcode);

//   Serial.print(F("📦 Raw name bytes: "));
//   for (int i = 0; i < sizeof(request->name); ++i) {
//     Serial.print((char)request->name[i]);
//   }
//   Serial.println();

//   Serial.print(F("📦 Interpreted name: '"));
//   Serial.print(request->name);
//   Serial.println("'");

//   if (request->opcode != OPCODE_ENTITY_COLOR || strlen(request->name) == 0) {
//     Serial.println(F("❌ Invalid or empty entity color packet"));
//     return "";
//   }

//   return String(request->name);
// }

const bool Receive::showTargetColorRequest(const byte* buffer) {
  const ShowTargetColorRequestPacket* request = reinterpret_cast<const ShowTargetColorRequestPacket*>(buffer);
  return request->switchOn;
}