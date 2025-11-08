#include "Send.h"
#include <RF24.h>
#include <OPCodes.h>
#include <Protocol.h>
#include "HubConfig.h"

Send::Send(RF24& radio, PairingRegistry& registry)
  : radio(radio), registry(registry) {}

const bool Send::toTargetPipe(uint8_t id, const uint8_t* pipe, const void* packet, uint8_t length) {
  radio.stopListening();
  radio.openWritingPipe(pipe);
  delay(5);  // ✅ Give target time to listen

  bool success = radio.write(packet, length);
  radio.startListening();

  if (!success) {
    Serial.print(F("❌ Failed to send data to target ID on pipe "));
    Serial.print(id);
    Serial.print(F(" on pipe "));
    Serial.println((char*)pipe);
    return false;
  }

  return true;
}

void Send::pairingRequest() {
  HubPairingRequestPacket packet = {
    OPCODE_PAIRING_POLL
  };

  Serial.print(F("📡 Hub sending pairing request on pairing poll pipe: 0x"));
  Serial.print((uint32_t)(pairingPollPipe >> 32), HEX);  // High 32 bits
  Serial.println((uint32_t)(pairingPollPipe & 0xFFFFFFFF), HEX);  // Low 32 bits

  radio.stopListening();
  radio.openWritingPipe(pairingPollPipe);
  // delay(50);
  bool success = radio.write(&packet, sizeof(packet));
  radio.openReadingPipe(1, pairingPipe);                                          // TO FIX MAYBE WRONG <--------------------------
  delay(100);  // ✅ Give target time to switch pipes
  radio.startListening();

  if (!success) {
    Serial.println(F("❌ Failed to send pairing request packet."));
    return;
  }

  Serial.println(F("✅ Sent pairing packet"));
}


void Send::pairingResponse(const TargetInfo& target) {
  PairingResponsePacket response = {
    OPCODE_PAIRING_RESPONSE,
    target.id,
    target.colorIndex
  };

  Serial.print(F("📡 Hub sending pairing response on pairing pipe: 0x"));
  Serial.print((uint32_t)(pairingPipe >> 32), HEX);  // High 32 bits
  Serial.println((uint32_t)(pairingPipe & 0xFFFFFFFF), HEX);  // Low 32 bits

  radio.stopListening();
  radio.openWritingPipe(pairingPipe);
  delay(50);
  bool success = radio.write(&response, sizeof(response));
  delay(100);  // ✅ Give target time to switch pipes
  radio.startListening();

  if (!success) {
    Serial.println(F("❌ Failed to send pairing response."));
    return;
  }

  Serial.print(F("✅ Sent pairing response with ID: "));
  Serial.print(target.id);
  Serial.print(F(" and color index: "));
  Serial.println(target.colorIndex);
}

void Send::verificationResponse(uint8_t id) {
  VerificationResponsePacket response = {
    OPCODE_VERIFICATION_RESPONSE,
    id
  };

  const uint8_t* pipe = registry.getPipeForID(id);
  Serial.print(F("📤 Trying to send verification response to target ID "));
  Serial.print(id);
  Serial.print(F(" via pipe "));
  Serial.println((char*)pipe);
  bool success = toTargetPipe(id, pipe, &response, sizeof(response));

  if (!success) {
    Serial.print(F("❌ Failed to send verification response for ID: "));
    Serial.println(id);
    return;
  }

  Serial.print(F("✅ Sent verification response for ID: "));
  Serial.println(id);
}

void Send::blinkAll(PairingRegistry& registry) {
  Serial.println(F("🔦 Triggering blink on all paired targets..."));

  BlinkCommandPacket packet;
  packet.opcode = OPCODE_BLINK_COMMAND;

  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      Serial.print(F("🔦 Sending blink to ID: "));
      Serial.println(id);
      toTargetPipe(id, pipe, &packet, sizeof(packet));
    }
  }
}

void Send::heartbeatAll(PairingRegistry& registry) {
  HeartbeatPacket packet = {
    OPCODE_HEARTBEAT
  };

  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    // Serial.print(F("Index: "));
    // Serial.print(i);
    
    uint8_t id = registry.getIDAt(i);
    // Serial.print(F(" - id: "));
    // Serial.println(id);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      toTargetPipe(id, pipe, &packet, sizeof(packet));
    }
  }
}

const bool Send::scoreUpdate(uint8_t id, const uint8_t* pipe, ScoreUpdated result) {
  HitResponsePacket packet = {
    OPCODE_SCORE_UPDATE,
    result.newScore,
    result.status
  };

  if (toTargetPipe(id, pipe, &packet, sizeof(packet))) {
    Serial.print(F("📤 Score update sent to target ID "));
    Serial.print(id);
    Serial.print(F(": "));
    Serial.println(result.newScore);
    return true;
  }
  else {
    return false;
  }
}

// const bool Send::entityColorRequest(uint8_t id, char colorName[16]) {
//   EntityColorRequestPacket packet;
//   packet.opcode = OPCODE_ENTITY_COLOR;
//   strncpy(packet.name, colorName, sizeof(packet.name));
//   packet.name[sizeof(packet.name) - 1] = '\0';

//   const uint8_t* pipe = registry.getPipeForID(id);
//   if (id != 0xFF && pipe) {
//     if (toTargetPipe(id, pipe, &packet, sizeof(packet))) {
//       Serial.print(F("🌈 Entity color \""));
//       Serial.print(colorName);
//       Serial.print(F("\" sent to target ID"));
//       Serial.println(id);
//       return true;
//     }
//   }
//   return false;
// }
const bool Send::targetSessionInfoRequest(uint8_t id, const TargetSessionInfo& sessionInfo) {
  TargetSessionInfoRequestPacket packet = {
    OPCODE_TARGET_SESSION_INFO,
    sessionInfo
  };

  const uint8_t* pipe = registry.getPipeForID(id);
  if (id != 0xFF && pipe) {
    if (toTargetPipe(id, pipe, &packet, sizeof(packet))) {
      Serial.print(F("🌈 Target Session Infos sent to target ID: "));
      Serial.println(id);
      return true;
    }
  }
  return false;
}

const bool Send::showTargetColorRequest(uint8_t id, bool switchOn) {
  ShowTargetColorRequestPacket packet = {
    OPCODE_SHOW_TARGET_COLOR,
    switchOn
  };

  const uint8_t* pipe = registry.getPipeForID(id);
  if (id != 0xFF && pipe) {
    if (toTargetPipe(id, pipe, &packet, sizeof(packet))) {
      Serial.print(F("🌈 Target color is "));
      Serial.println(switchOn == true ? "ON" : "OFF");
      return true;
    }
  }
  return false;
}

const bool Send::sessionStatusRequest(uint8_t id, GameSessionStatus status) {
  SessionStatusPacket packet = {
    OPCODE_SESSION_STATUS,
    status
  };

  const uint8_t* pipe = registry.getPipeForID(id);
  if (id != 0xFF && pipe) {
    if (toTargetPipe(id, pipe, &packet, sizeof(packet))) {
      // Serial.print(F("📤 Status update sent to target ID "));
      // Serial.println(id);
      return true;
    }
  }
  return false;
}

const bool Send::litTargetRequest(uint8_t id, uint8_t indexInEntity) {
  GameModeLitTargetRequestPacket packet = {
    OPCODE_GAMEMODE_LITTARGET,
    indexInEntity
  };

  const uint8_t* pipe = registry.getPipeForID(id);
  if (id != 0xFF && pipe) {
    if (toTargetPipe(id, pipe, &packet, sizeof(packet))) {
      Serial.print(F("📤 litTarget update sent to target ID "));
      Serial.println(id);
      return true;
    }
  }
  return false;
}


