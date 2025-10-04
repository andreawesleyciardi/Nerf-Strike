#include "Receive.h"
#include <Protocol.h>
#include <OPCodes.h>

Receive::Receive(TargetTypeManager& typeMgr, PairingRegistry& reg)
  : targetTypeManager(typeMgr), registry(reg) {}

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

const uint8_t* Receive::hitPacket(const byte* buffer) {
  HitPacket* packet = reinterpret_cast<HitPacket*>(const_cast<byte*>(buffer));
  uint8_t targetId = packet->id;

  Serial.print(F("🎯 Hit received from target ID: "));
  Serial.println(targetId);

  const uint8_t* pipe = registry.getPipeForID(targetId);
  return pipe;
}