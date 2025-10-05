#include "Send.h"
#include <RF24.h>
#include <OPCodes.h>
#include "HubConfig.h"

Send::Send(RF24& radio) : radio(radio) {}

void Send::pairingResponse(uint8_t assignedID, TargetType type) {
  PairingResponsePacket response = {
    OPCODE_PAIRING_RESPONSE,
    assignedID,
    type
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

  if (success) {
    Serial.print(F("✅ Sent pairing response with ID: "));
    Serial.print(assignedID);
    Serial.print(F(" | Type: "));
    Serial.println(targetTypeToString(type));
  } else {
    Serial.println(F("❌ Failed to send pairing response."));
  }
}

void Send::verificationResponse(uint8_t id) {
  VerificationResponsePacket response = {
    OPCODE_VERIFICATION_RESPONSE,
    id
  };

  radio.stopListening();
  radio.openWritingPipe(pairingPipe);
  bool success = radio.write(&response, sizeof(response));
  radio.startListening();

  if (success) {
    Serial.print(F("✅ Sent verification response for ID: "));
    Serial.println(id);
  } else {
    Serial.print(F("❌ Failed to send verification response for ID: "));
    Serial.println(id);
  }
}

void Send::toTargetPipe(uint8_t id, const uint8_t* pipe, const void* packet, uint8_t length) {
  radio.stopListening();
  radio.openWritingPipe(pipe);
  delay(5);  // ✅ Give target time to listen

  bool success = radio.write(packet, length);
  radio.startListening();

  if (success) {
    Serial.print("📤 Sent data to target ID ");
    Serial.print(id);
    Serial.print(" via pipe ");
    Serial.println((char*)pipe);
  } else {
    Serial.println(F("❌ Failed to send data to target."));
  }
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
  HeartbeatPacket packet;
  packet.opcode = OPCODE_HEARTBEAT;

  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      toTargetPipe(id, pipe, &packet, sizeof(packet));
    }
  }
}

void Send::scoreUpdate(uint8_t id, const uint8_t* pipe, uint8_t newScore) {
  ScoreUpdatePacket packet = {
    OPCODE_SCORE_UPDATE,
    newScore
  };

  toTargetPipe(id, pipe, &packet, sizeof(packet));
  Serial.print(F("📤 Score update sent to target ID "));
  Serial.print(id);
  Serial.print(F(": "));
  Serial.println(newScore);
}