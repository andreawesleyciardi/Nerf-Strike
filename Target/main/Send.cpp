#include "Send.h"
#include <RF24.h>
#include <OPCodes.h>
#include <Protocol.h>
#include "TargetConfig.h"

Send::Send(RF24& radio, PairingRegistry& registry)
  : radio(radio), registry(registry) {}

const bool Send::toHub(const byte* data, uint8_t length, bool onTargetPipe) {
  Serial.print(F("📤 Sending data to hub on pipe: "));
  radio.stopListening();
  if (onTargetPipe == true) {
    const uint8_t* targetPipe = registry.getTargetPipe();
    Serial.println((char*)targetPipe);
    radio.openWritingPipe(targetPipe);
  } else {
    Serial.print((uint32_t)(pairingPipe >> 32), HEX);  // High 32 bits
    Serial.println((uint32_t)(pairingPipe & 0xFFFFFFFF), HEX);  // Low 32 bits
    radio.openWritingPipe(pairingPipe);
  }
  bool success = radio.write(data, length);
  radio.startListening();

  if (!success) {
    Serial.println(F("❌ Failed to send data to hub."));
    return false;
  }
  Serial.println(F("✅ Sent data to hub via target pipe."));
  return true;
}

const bool Send::pairingRequest(uint32_t token) {
  PairingRequestPacket request = {
    OPCODE_PAIRING_REQUEST,
    token,
    targetType  // from TargetConfig.h
  };

  Serial.print(F("📨 Sending active pairing request with token: "));
  Serial.println(token);

  return toHub(reinterpret_cast<const byte*>(&request), sizeof(request), false);
}

const bool Send::verificationRequest(uint8_t id) {
  VerificationRequestPacket request = {
    OPCODE_VERIFICATION_REQUEST,
    id
  };

  Serial.print(F("📨 Sending verification request for ID "));
  Serial.println(id);

  bool success = toHub(reinterpret_cast<const byte*>(&request), sizeof(request), false);

  if (!success) {
    Serial.println(F("❌ Failed to send verification request."));
    return false;
  }
  Serial.println(F("✅ Verification request sent."));
  return true;
}

const bool Send::hitRequest(uint8_t targetId) {
  HitRequestPacket request = {
    OPCODE_HIT_REQUEST,
    targetId
  };

  bool success = toHub(reinterpret_cast<const byte*>(&request), sizeof(request));

  if (!success) {
    Serial.println(F("❌ Failed to send hit request."));
    return false;
  }
  Serial.println(F("📡 Hit request sent successfully."));
  return true;
}

// void Send::toHub(const byte* data, uint8_t length) {
//   radio.stopListening();
//   radio.openWritingPipe(targetPipe);
//   bool success = radio.write(data, length);
//   radio.startListening();

//   if (success) {
//     Serial.println(F("📤 Sent data to hub via target pipe."));
//   } else {
//     Serial.println(F("❌ Failed to send data to hub."));
//   }
// }