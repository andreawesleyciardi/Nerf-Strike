#include "Send.h"
#include <RF24.h>
#include <OPCodes.h>

#include "TargetConfig.h"

Send::Send(RF24& radio, PairingRegistry& registry)
  : radio(radio), registry(registry) {}

const bool Send::toHub(const byte* data, uint8_t length, String label, bool logging, OnPipe onPipe) {
  if (logging) {
    Serial.println();
    Serial.println("📤 Sending " + label + " to Hub");
  }
  radio.stopListening();
  if (onPipe == OnPipe::pairingPipe) {
    if (logging) {
      Serial.print(F("🧬 Communicating on Pairing Pipe: "));
      Serial.print((uint32_t)(pairingPipe >> 32), HEX);  // High 32 bits
      Serial.println((uint32_t)(pairingPipe & 0xFFFFFFFF), HEX);  // Low 32 bits
    }
    radio.openWritingPipe(pairingPipe);
  }
  // else if (onPipe == OnPipe::targetPipe) {                           // Targets write always on pairingPipe
  //   const uint8_t* targetPipe = registry.getTargetPipe();
  //   if (logging) {
  //     Serial.print(F("🧬 Communicating on Target Pipe: "));
  //     Serial.println((char*)targetPipe);
  //   }
  //   radio.openWritingPipe(targetPipe);
  // }
  bool success = radio.write(data, length);
  radio.startListening();

  if (!success) {
    if (logging) {
      Serial.println("❌ Failed to send " + label + " to Hub.");
    }
    return false;
  }
  if (logging) {
    Serial.println("✅ Sent " + label + " to Hub.");
  }
  return true;
}

// ************************************************************************************************************************************************************************************************

const bool Send::pairingRequest(uint32_t token) {
  PairingRequestPacket request = {
    OPCODE_PAIRING_REQUEST,
    token,
    targetType  // from TargetConfig.h
  };

  Serial.println();
  Serial.print(F("🧾 Token: "));
  Serial.println(token);

  return toHub(reinterpret_cast<const byte*>(&request), sizeof(request), "pairing request");
}

const bool Send::verificationRequest(uint8_t id) {
  VerificationRequestPacket request = {
    OPCODE_VERIFICATION_REQUEST,
    id
  };

  return toHub(reinterpret_cast<const byte*>(&request), sizeof(request), "verification request");
}

// const bool Send::heartbeatResponse(uint8_t targetId) {
//   HeartbeatResponsePacket response = {
//     OPCODE_HEARTBEAT_RESPONSE,
//     targetId
//   };

//   return toHub(reinterpret_cast<const byte*>(&response), sizeof(response), "heartbeat", false);
// }

const bool Send::hitRequest(uint8_t targetId) {
  HitRequestPacket request = {
    OPCODE_HIT_REQUEST,
    targetId
  };

  return toHub(reinterpret_cast<const byte*>(&request), sizeof(request), "hit request");
}