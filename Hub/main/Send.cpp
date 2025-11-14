#include "Send.h"
#include <RF24.h>
#include <OPCodes.h>
#include <Protocol.h>
#include "HubConfig.h"

Send::Send(RF24& radio, PairingRegistry& registry)
  : radio(radio), registry(registry) {}

const bool Send::toTarget(uint8_t id, const void* packet, size_t packetSize, String label, bool logging) {
  const uint8_t* pipe = registry.getPipeForID(id);
  if (!pipe) {
    Serial.println();
    Serial.println("❌ It was not possible to send " + label + " to Target.");
    Serial.print(F("🆔 No pipe found for ID: "));
    Serial.println(id);
    return false;
  }

  radio.stopListening();
  radio.openWritingPipe(pipe);
  delay(5);  // Give target time to listen
  bool success = radio.write(packet, packetSize);
  radio.startListening();

  if (logging) {
    Serial.println();
    if (!success) {
      Serial.println("❌ Failed to send " + label + " to Target:");
    }
    else {
      Serial.println("📤 Sent " + label + " to Target:");
    }
    Serial.print(F("🆔 Id: "));
    Serial.println(id);
    Serial.print(F("🧬 Pipe: "));
    Serial.println((char*)pipe);
  }

  return success;
}

const bool Send::toAllTargets(const void* packet, size_t packetSize, String label, bool logging) {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    if (id != 0xFF) {
      toTarget(id, packet, packetSize, label, logging);
    }
  }
}

// ************************************************************************************************************************************************************************************************

const bool Send::pairingSollecitation() {
  PairingSollecitationPacket packet = {
    OPCODE_PAIRING_SOLLECITATION
  };

  Serial.println();
  Serial.print(F("📤 Sending pairing sollecitation on pairing pipe: 0x"));
  Serial.print((uint32_t)(pairingPipe >> 32), HEX);  // High 32 bits
  Serial.println((uint32_t)(pairingPipe & 0xFFFFFFFF), HEX);  // Low 32 bits

  radio.stopListening();
  radio.openWritingPipe(pairingPipe);
  delay(5); // Give target time to listen
  bool success = radio.write(&packet, sizeof(packet));
  radio.startListening();

  Serial.println();
  if (!success) {
    Serial.println(F("❌ Failed to send pairing sollecitation."));
    return false;
  }

  Serial.println(F("✅ Sent pairing sollecitation"));
  return true;
}

void Send::pairingResponse(const TargetInfo& target) {
  PairingResponsePacket response = {
    OPCODE_PAIRING_RESPONSE,
    target.token,
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

  Serial.println();
  Serial.print(F("✅ Sent pairing response."));
  Serial.print(F("🆔 Id: "));
  Serial.println(target.id);
  Serial.print(F("🌈 Color index: "));
  Serial.println(target.colorIndex);
}

void Send::verificationResponse(uint8_t id) {
  VerificationResponsePacket response = {
    OPCODE_VERIFICATION_RESPONSE,
    id
  };
  toTarget(id, &response, sizeof(response), "verification response");
}

void Send::blinkAll() {
  BlinkCommandPacket packet = {
    OPCODE_BLINK_COMMAND
  };

  toAllTargets(&packet, sizeof(packet), "blink");
}

void Send::heartbeatAll() {
  HeartbeatRequestPacket packet = {
    OPCODE_HEARTBEAT_REQUEST
  };

  const unsigned long TIMEOUT = 10000; // 10 seconds
  unsigned long now = millis();

  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    if (id != 0xFF) {
      TargetInfo& info = registry.getTargetRefByID(id);
      bool success = toTarget(id, &packet, sizeof(packet), "heartbeat", false);

      if (success) {
        // Target responded to heartbeat — clear unreachable flag
        if (info.unreachableFrom != 0) {
          Serial.print(F("🎯 Target "));
          Serial.print(id);
          Serial.println(F(" responded again. Clearing unreachable flag."));
          info.unreachableFrom = 0;
        }
      } else {
        // Failed to send heartbeat — track unreachable time
        if (info.unreachableFrom == 0) {
          info.unreachableFrom = now;
          Serial.print(F("⚠️ First failed heartbeat to Target "));
          Serial.println(id);
        } else if (now - info.unreachableFrom > TIMEOUT) {
          Serial.print(F("❌ Target "));
          Serial.print(id);
          Serial.println(F(" has been unreachable for too long. Removing..."));
          registry.removeTargetById(id);
        }
      }
    }
  }
}

const bool Send::scoreUpdate(uint8_t id, ScoreUpdated result) {
  HitResponsePacket packet = {
    OPCODE_SCORE_UPDATE,
    result.newScore,
    result.status
  };

  Serial.println();
  Serial.print(F("⭐ New score: "));
  Serial.println(result.newScore);

  return toTarget(id, &packet, sizeof(packet), "score update");
}

// const bool Send::entityColorRequest(uint8_t id, char colorName[16]) {
//   EntityColorRequestPacket packet;
//   packet.opcode = OPCODE_ENTITY_COLOR;
//   strncpy(packet.name, colorName, sizeof(packet.name));
//   packet.name[sizeof(packet.name) - 1] = '\0';

//   const uint8_t* pipe = registry.getPipeForID(id);
//   if (id != 0xFF && pipe) {
//     if (toTarget(id, pipe, &packet, sizeof(packet))) {
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

  return toTarget(id, &packet, sizeof(packet), "Target session infos");
}

const bool Send::showTargetColorRequest(uint8_t id, bool switchOn) {
  IdentifyTargetRequestPacket packet = {
    OPCODE_IDENTIFY_TARGET,
    switchOn
  };

  if (toTarget(id, &packet, sizeof(packet))) {
    Serial.print(F("🌈 Target color is "));
    Serial.println(switchOn == true ? "ON" : "OFF");
    return true;
  }
  return false;
}

const bool Send::sessionStatusRequest(uint8_t id, GameSessionStatus status) {
  SessionStatusPacket packet = {
    OPCODE_SESSION_STATUS,
    status
  };

  return toTarget(id, &packet, sizeof(packet), "session infos");
}

const bool Send::litTargetRequest(uint8_t id, uint8_t indexInEntity) {
  GameModeLitTargetRequestPacket packet = {
    OPCODE_GAMEMODE_LITTARGET,
    indexInEntity
  };

  return toTarget(id, &packet, sizeof(packet), "litTarget update");
}


