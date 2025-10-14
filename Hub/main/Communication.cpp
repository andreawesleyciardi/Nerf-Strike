#include "Communication.h"
#include <Protocol.h>
#include <OPCodes.h>
#include <DisplayFeedback.h>

Communication::Communication(Receive& receive, Send& send, PairingRegistry& registry, GameLogic& gameLogic, RGBLed& statusRgbLed)
  : receive(receive), send(send), registry(registry), gameLogic(gameLogic), statusRgbLed(statusRgbLed) {}

const uint8_t* Communication::verifyPipeForID(uint8_t targetId) {
  const uint8_t* pipe = registry.getPipeForID(targetId);
  if (!pipe) {
    Serial.print(F("❌ No pipe found for ID: "));
    Serial.println(targetId);
    showStatus(statusRgbLed, STATUS_ERROR);
  }
  return pipe;
}

void Communication::pairing(const byte* buffer) {
  const uint8_t assignedID = receive.pairingRequest(buffer);
  if (assignedID == 0xFF) {
    Serial.println(F("❌ Failed to assign ID."));
    showStatus(statusRgbLed, STATUS_ERROR, 2);
    return;
  }

  send.pairingResponse(assignedID);

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

const bool Communication::hit(const byte* buffer) {
  HitRequestPacket request = receive.hitRequest(buffer);
  const uint8_t* pipe = verifyPipeForID(request.id);
  if (pipe) {
    uint8_t newScore = gameLogic.updateEntityScore(request.id);
    return send.hitResponse(request.id, pipe, newScore);
  }
  return false;
}

const bool Communication::entityColor(uint8_t targetId, String colorName) {
  Serial.println(F("Communication::entityColor"));
  return send.entityColorRequest(targetId, colorName);
}