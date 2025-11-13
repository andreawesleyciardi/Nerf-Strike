#include "Send.h"
#include <RF24.h>
#include <OPCodes.h>

#include "TargetConfig.h"

Send::Send(RF24& radio, PairingRegistry& registry)
  : radio(radio), registry(registry) {}

const bool Send::toHub(const byte* data, uint8_t length, OnPipe onPipe) {
  Serial.println();
  Serial.print(F("📤 Sending data to hub on "));
  radio.stopListening();
  if (onPipe == OnPipe::pairingPipe) {
    Serial.print(F("Pairing Pipe: "));
    Serial.print((uint32_t)(pairingPipe >> 32), HEX);  // High 32 bits
    Serial.println((uint32_t)(pairingPipe & 0xFFFFFFFF), HEX);  // Low 32 bits
    radio.openWritingPipe(pairingPipe);
  }
  else if (onPipe == OnPipe::targetPipe) {
    const uint8_t* targetPipe = registry.getTargetPipe();
    Serial.print(F("Target Pipe: "));
    Serial.println((char*)targetPipe);
    radio.openWritingPipe(targetPipe);
  }
  bool success = radio.write(data, length);
  radio.startListening();

  if (!success) {
    Serial.println(F("❌ Failed to send data to hub."));
    return false;
  }
  Serial.println(F("✅ Sent data to hub."));
  return true;
}

const bool Send::pairingRequest(uint32_t token) {
  PairingRequestPacket request = {
    OPCODE_PAIRING_REQUEST,
    token,
    targetType  // from TargetConfig.h
  };

  Serial.println();
  Serial.print(F("📤 Sending active pairing request with token: "));
  Serial.println(token);

  return toHub(reinterpret_cast<const byte*>(&request), sizeof(request));
}

const bool Send::verificationRequest(uint8_t id) {
  VerificationRequestPacket request = {
    OPCODE_VERIFICATION_REQUEST,
    id
  };

  Serial.print(F("📨 Sending verification request for ID "));
  Serial.println(id);

  bool success = toHub(reinterpret_cast<const byte*>(&request), sizeof(request));

  if (!success) {
    Serial.println(F("❌ Failed to send verification request."));
    return false;
  }
  Serial.println(F("✅ Verification request sent."));
  return true;
}

const bool Send::heartbeatResponse() {
  HeartbeatPacket response = {
    OPCODE_HEARTBEAT
  };

  return toHub(reinterpret_cast<const byte*>(&response), sizeof(response));
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