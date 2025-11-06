#include "Receive.h"
// #include <Protocol.h>
// #include <OPCodes.h>

Receive::Receive(TargetTypeManager& targetTypeManager, PairingRegistry& registry)
  : targetTypeManager(targetTypeManager), registry(registry) {}

TargetInfo Receive::pairingRequest(const byte* buffer) {
  const PairingRequestPacket* request = reinterpret_cast<const PairingRequestPacket*>(buffer);
  const uint32_t token = request->token;
  const TargetType incomingType = request->type;

  Serial.print(F("🔐 Pairing request received with token: "));
  Serial.println(token);
  Serial.print(F("📦 Incoming target type: "));
  Serial.println(targetTypeToString(incomingType));

  // 🔒 Validate target type compatibility
  if (!targetTypeManager.isCompatible(incomingType)) {
    Serial.print(F("❌ Target type mismatch. Expected "));
    Serial.print(targetTypeToString(targetTypeManager.getAllowedType()));
    Serial.print(F(", but got "));
    Serial.println(targetTypeToString(incomingType));
    return TargetInfo();  // Return default (invalid)
  }

  // 🔍 Check if token is already paired
  int existingIndex = registry.findTargetByToken(token);
  TargetInfo target;

  if (existingIndex != -1) {
    // ✅ Reuse existing TargetInfo
    target = registry.getTargetByID(registry.getIDAt(existingIndex));
    Serial.print(F("🔁 Re-pairing known target with ID: "));
    Serial.println(target.id);
  } else {
    // 🆕 Assign new ID and color
    target = registry.setTarget(token);
    if (!target.isValid()) {
      Serial.println(F("❌ Failed to assign target info."));
      return TargetInfo();
    }

    // 🧬 Assign pipe name
    char pipeName[6];
    sprintf(pipeName, "TGT%d", target.id);
    memcpy(target.pipe, reinterpret_cast<uint8_t*>(pipeName), 6);

    // 💾 Store full target info
    registry.storeTargetInfo(target);

    Serial.print(F("✅ Assigned new ID: "));
    Serial.print(target.id);
    Serial.print(F(" with color index: "));
    Serial.println(target.colorIndex);
    Serial.print(F("📡 Assigned pipe: "));
    Serial.println(pipeName);
  }

  return target;
}

// TargetInfo Receive::pairingRequest(const byte* buffer) {
//   const PairingRequestPacket* request = reinterpret_cast<const PairingRequestPacket*>(buffer);
//   const uint32_t token = request->token;
//   const TargetType incomingType = request->type;

//   Serial.print(F("🔐 Pairing request received with token: "));
//   Serial.println(token);
//   Serial.print(F("📦 Incoming target type: "));
//   Serial.println(targetTypeToString(incomingType));

//   if (!targetTypeManager.isCompatible(incomingType)) {
//     Serial.print(F("❌ Target type mismatch. Expected "));
//     Serial.print(targetTypeToString(targetTypeManager.getAllowedType()));
//     Serial.print(F(", but got "));
//     Serial.println(targetTypeToString(incomingType));
//     return TargetInfo();  // Return default (invalid)
//   }

//   TargetInfo target = registry.setTarget(token);  // ✅ Assign ID and color
//   if (!target.isValid()) {
//     Serial.println(F("❌ Failed to assign target info."));
//   } else {
//     Serial.print(F("✅ Assigned ID: "));
//     Serial.print(target.id);
//     Serial.print(F(" with color index: "));
//     Serial.println(target.colorIndex);
//   }

//   return target;
// }

const uint8_t* Receive::verificationRequest(const byte* buffer) {
  VerificationRequestPacket* request = reinterpret_cast<VerificationRequestPacket*>(const_cast<byte*>(buffer));

  Serial.print(F("🔍 Received verification request for ID: "));
  Serial.println(request->id);

  const uint8_t* pipe = registry.getPipeForID(request->id);
  return pipe;
}

HitRequestPacket Receive::hitRequest(const byte* buffer) {
  HitRequestPacket* request = reinterpret_cast<HitRequestPacket*>(const_cast<byte*>(buffer));

  Serial.print(F("🎯 Hit received from target ID: "));
  Serial.println(request->id);

  return *request;
}