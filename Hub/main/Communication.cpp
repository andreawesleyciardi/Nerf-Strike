#include "Communication.h"
#include <Protocol.h>
#include <OPCodes.h>
#include <DisplayFeedback.h>

Communication::Communication(Receive& receive, Send& send, PairingRegistry& registry, GameLogic& gameLogic, RGBLed& statusRgbLed)
  : receive(receive), send(send), registry(registry), gameLogic(gameLogic), statusRgbLed(statusRgbLed) {}

void Communication::pairing(const byte* buffer) {
  const uint8_t assignedID = receive.pairingRequest(buffer);
  if (assignedID == 0xFF) {
    Serial.println(F("❌ Failed to assign ID."));
    showStatus(statusRgbLed, STATUS_ERROR, 2);
    return;
  }

  const PairingRequestPacket* request = reinterpret_cast<const PairingRequestPacket*>(buffer);
  TargetType incomingType = request->type;
  send.pairingResponse(assignedID, incomingType);

  char pipeName[6];
  sprintf(pipeName, "TGT%d", assignedID);
  registry.storePipeForID(assignedID, reinterpret_cast<uint8_t*>(pipeName));

  Serial.print(F("📡 Stored pipe for ID "));
  Serial.print(assignedID);
  Serial.print(F(": "));
  Serial.println(pipeName);

  showStatus(statusRgbLed, STATUS_PAIRING, 2);
  delay(100);
}

void Communication::verification(const byte* buffer) {
  const uint8_t* pipe = receive.verificationRequest(buffer);
  if (!pipe) {
    Serial.println(F("❌ No pipe found for ID."));
    showStatus(statusRgbLed, STATUS_ERROR);
    return;
  }

  const VerificationRequestPacket* request = reinterpret_cast<const VerificationRequestPacket*>(buffer);
  uint8_t targetId = request->id;
  Serial.println(F("✅ Pipe found. Sending verification ACK."));
  send.verificationResponse(targetId);
}

void Communication::hit(const byte* buffer) {
  const uint8_t* pipe = receive.hitEvent(buffer);
  if (!pipe) {
    Serial.println(F("❌ No pipe found for target ID."));
    showStatus(statusRgbLed, STATUS_ERROR);
    return;
  }

  const HitEventPacket* request = reinterpret_cast<const HitEventPacket*>(buffer);
  uint8_t targetId = request->id;
  uint8_t newScore = gameLogic.incrementScoreFor(targetId);
  send.scoreUpdate(targetId, pipe, newScore);
}