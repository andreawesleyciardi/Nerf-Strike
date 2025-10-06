#include "Receive.h"
#include <Protocol.h>
#include <OPCodes.h>

Receive::Receive(TargetTypeManager& targetTypeManager, PairingRegistry& registry)
  : targetTypeManager(targetTypeManager), registry(registry) {}

const uint8_t* Receive::verificationRequest(const byte* buffer) {
  VerificationRequestPacket* request = reinterpret_cast<VerificationRequestPacket*>(const_cast<byte*>(buffer));

  Serial.print(F("🔍 Received verification request for ID: "));
  Serial.println(request->id);

  const uint8_t* pipe = registry.getPipeForID(request->id);
  return pipe;
}

const uint8_t Receive::pairingRequest(const byte* buffer) {
  const PairingRequestPacket* request = reinterpret_cast<const PairingRequestPacket*>(buffer);
  const uint32_t token = request->token;
  const TargetType incomingType = request->type;

  Serial.print(F("🔐 Pairing request received with token: "));
  Serial.println(token);
  Serial.print(F("📦 Incoming target type: "));
  Serial.println(targetTypeToString(incomingType));

  if (!targetTypeManager.isCompatible(incomingType)) {
    Serial.print(F("❌ Target type mismatch. Expected "));
    Serial.print(targetTypeToString(targetTypeManager.getAllowedType()));
    Serial.print(F(", but got "));
    Serial.println(targetTypeToString(incomingType));
    return 0xFF;
  }

  const uint8_t assignedID = registry.assignID(token);
  return assignedID;
}

const uint8_t* Receive::hitEvent(const byte* buffer) {
  HitEventPacket* packet = reinterpret_cast<HitEventPacket*>(const_cast<byte*>(buffer));

  Serial.print(F("🎯 Hit received from target ID: "));
  Serial.println(packet->id);

  const uint8_t* pipe = registry.getPipeForID(packet->id);
  return pipe;
}