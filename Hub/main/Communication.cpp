#include "Communication.h"

Communication::Communication(Receive& receive, Send& send, PairingRegistry& registry, GameLogic& gameLogic, RGBLed& statusRgbLed, GameSessionManager& sessionManager)
  : receive(receive), send(send), registry(registry), gameLogic(gameLogic), statusRgbLed(statusRgbLed), sessionManager(sessionManager) {}

const uint8_t* Communication::verifyPipeForID(uint8_t targetId) {
  const uint8_t* pipe = registry.getPipeForID(targetId);
  if (!pipe) {
    Serial.print(F("❌ No pipe found for ID: "));
    Serial.println(targetId);
    showStatus(statusRgbLed, STATUS_ERROR);
  }
  return pipe;
}

void Communication::pairing(const byte* buffer) {                 // TODO: Transform in boolean and move outside this: showStatus(statusRgbLed, STATUS_PAIRING, 2); 
  TargetInfo target = receive.pairingRequest(buffer);
  if (!target.isValid()) {
    Serial.println(F("❌ Failed to assign ID."));
    showStatus(statusRgbLed, STATUS_ERROR, 2);
    return;
  }

  // Assign pipe name and copy into target.pipe
  char pipeName[6];
  sprintf(pipeName, "TGT%d", target.id);
  memcpy(target.pipe, reinterpret_cast<uint8_t*>(pipeName), 6);  // ✅ Fill pipe field

  // Store full target info (pipe + color)
  registry.storeTargetInfo(target);

  // Send pairing response with full TargetInfo
  send.pairingResponse(target);

  Serial.print(F("📡 Stored pipe for ID "));
  Serial.print(target.id);
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

bool Communication::updateBatch(ScoreUpdateBatch batch, uint8_t id) {
  bool sent = false;
  for (uint8_t i = 0; i < batch.count; ++i) {
    const ScoreUpdatedPerTarget& result = batch.updates[i];
    const uint8_t* targetPipe = verifyPipeForID(result.targetId);
    if (targetPipe) {
      const bool wasSent = send.scoreUpdate(result.targetId, targetPipe, {result.newScore, result.status});
      if (id != TARGET_ID_NONE && result.targetId == id) {
        sent = wasSent;
      }
      else {
        sent = true;
      }
    }
  }
  return sent;
}

const bool Communication::hit(const byte* buffer) {
  HitRequestPacket request = receive.hitRequest(buffer);
  const uint8_t* pipe = verifyPipeForID(request.id);
  if (pipe) {
    ScoreUpdateBatch batch = gameLogic.updateEntityScore(request.id);
    if (batch.count > 0) {
      return updateBatch(batch);
    }
  }
  return false;
}

const bool Communication::litTarget(uint8_t indexInEntity) {
  const EntityInfo* entities = sessionManager.getAllEntities();
  uint8_t entityCount = sessionManager.getEntityCount();

  bool result = true;
  for (uint8_t i = 0; i < entityCount; ++i) {
    for (uint8_t j = 0; j < entities[i].targetCount; ++j) {
      if (!send.litTargetRequest(entities[i].targetIds[j], indexInEntity)) {
        result = false;
      }
    }
  }
  return result;
}

const bool Communication::alertGameTimerEnded(ScoreUpdateBatch batch) {
  return updateBatch(batch);
}



// const bool Communication::entityColor(uint8_t targetId, char colorName[16]) {
//   return send.entityColorRequest(targetId, colorName);
// }
const bool Communication::targetSessionInfo(uint8_t targetId, const TargetSessionInfo& sessionInfo) {
  return send.targetSessionInfoRequest(targetId, sessionInfo);
}

const bool Communication::showTargetColor(uint8_t targetId, bool switchOn) {
  Serial.println(F("Communication::showTargetColor"));
  return send.showTargetColorRequest(targetId, switchOn);
}

const bool Communication::sessionStatus(uint8_t targetId, GameSessionStatus status) {
  return send.sessionStatusRequest(targetId, status);
}