#include "Communication.h"
#include <Protocol.h>
#include <OPCodes.h>
#include <DisplayFeedback.h>

Communication::Communication(Receive& receive, Send& send, PairingRegistry& registry, RGBLed& statusRgbLed)
  : receive(receive), send(send), registry(registry), statusRgbLed(statusRgbLed) {}

void Communication::pairing() {
  showStatus(statusRgbLed, STATUS_PAIRING);
  uint32_t token = registry.loadTokenFromEEPROM();

  if (token == 0xFFFFFFFF || token == 0) {
    token = random(100000, 999999);
    registry.saveTokenToEEPROM(token);
    Serial.print(F("🆕 Generated new token: "));
    Serial.println(token);
  } else {
    registry.setToken(token);
    Serial.print(F("🔁 Reusing stored token: "));
    Serial.println(token);
  }

  for (int attempt = 1; attempt <= 3; attempt++) {
    Serial.print(F("📨 Attempt "));
    Serial.print(attempt);
    Serial.println(F(" to pair..."));

    send.pairingRequest(token);

    const uint8_t assignedID = receive.pairingResponse();
    if (assignedID != 0xFF) {
      registry.setAssignedID(assignedID);
      // wireless.switchToTargetPipe(assignedID);
      Serial.print(F("✅ Paired successfully with ID: "));
      Serial.println(assignedID);
      return;
    }

    delay(500);
  }

  Serial.println(F("❌ Pairing failed after multiple attempts."));
  registry.setAssignedID(0xFF);
}

const bool Communication::verification() {
  uint8_t assignedID = registry.getAssignedID();
  if (assignedID == 0xFF) {
    Serial.println(F("⚠️ Cannot verify — no assigned ID."));
    showStatus(statusRgbLed, STATUS_ERROR);
    return false;
  }

  if (!send.verificationRequest(assignedID)) {
    showStatus(statusRgbLed, STATUS_ERROR);
    return false;
  }

  if (!receive.verificationResponse(assignedID)) {
    Serial.println(F("❌ Verification failed. Re-pairing..."));
    showStatus(statusRgbLed, STATUS_ERROR);
    pairing();
    return false;
  }

  Serial.println(F("✅ Connection verified."));
  showStatus(statusRgbLed, STATUS_CONNECTED);
  return true;
}

void Communication::hit() {
  
}