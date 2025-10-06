#include "Send.h"
#include <RF24.h>
#include <OPCodes.h>
#include <Protocol.h>
#include "TargetConfig.h"

Send::Send(RF24& radio) : radio(radio) {}

void Send::pairingRequest(uint32_t token) {
  PairingRequestPacket request = {
    OPCODE_PAIRING_REQUEST,
    token,
    targetType  // from TargetConfig.h
  };

  Serial.print(F("📨 Sending pairing request with token: "));
  Serial.println(token);

  radio.stopListening();
  radio.openWritingPipe(pairingPipe);
  bool success = radio.write(&request, sizeof(request));
  radio.startListening();

  if (!success) {
    Serial.println(F("❌ Failed to send pairing request."));
    return;
  }
  Serial.println(F("✅ Pairing request sent."));
}

void Send::verificationRequest(uint8_t id) {
  
}

void Send::hitEvent(uint8_t assignedID) {
  
}

void Send::toHub(const byte* data, uint8_t length) {

}