#include "Communication.h"
#include <Protocol.h>
#include <OPCodes.h>
#include <DisplayFeedback.h>

Communication::Communication(Receive& receive, Send& send, PairingRegistry& registry, RGBLed& statusRgbLed)
  : receive(receive), send(send), registry(registry), statusRgbLed(statusRgbLed) {}

TargetInfo Communication::verifyAssignedTarget() {
  TargetInfo target = registry.getTargetInfo();
  if (!target.isValid()) {
    Serial.println(F("⚠️ Cannot verify — no valid assigned target."));
    showStatus(statusRgbLed, STATUS_ERROR);
  }
  return target;
}

const bool Communication::pairingRequest(uint32_t token) {
  showStatus(statusRgbLed, STATUS_PAIRING);

  if (token == 0xFFFFFFFF || token == 0) {
    token = registry.loadTokenFromEEPROM();
    if (token == 0xFFFFFFFF || token == 0) {
      token = random(100000, 999999);
      registry.saveTokenToEEPROM(token);
      Serial.print(F("🆕 Generated new token: "));
      Serial.println(token);
    } else {
      Serial.print(F("🔁 Reusing stored token: "));
      Serial.println(token);
    }
  } else {
    Serial.print(F("🔁 Using provided token: "));
    Serial.println(token);
  }

  registry.setToken(token);

  for (int attempt = 1; attempt <= 3; attempt++) {
    Serial.print(F("📨 Attempt "));
    Serial.print(attempt);
    Serial.println(F(" to pair..."));

    if (send.pairingRequest(token)) {
      Serial.println(F("✅ Pairing request sent."));
      TargetInfo target = receive.pairingResponse();
      if (target.isValid()) {
        target.token = token;
        registry.setTargetInfo(target);
        Serial.print(F("✅ Paired successfully with ID: "));
        Serial.print(target.id);
        Serial.print(F(" and color index: "));
        Serial.println(target.colorIndex);
        return true;
      } else {
        Serial.println(F("❌ No valid pairing response received."));
      }
    } else {
      Serial.println(F("❌ Failed to send pairing request."));
    }

    delay(500);
  }

  Serial.println(F("❌ Pairing failed after multiple attempts."));
  TargetInfo failed;
  failed.id = 0xFF;
  registry.setTargetInfo(failed);
  return false;
}

// const bool Communication::pairingRequest() {
//   showStatus(statusRgbLed, STATUS_PAIRING);
//   uint32_t token = registry.loadTokenFromEEPROM();

//   bool result = false;
//   if (token == 0xFFFFFFFF || token == 0) {
//     token = random(100000, 999999);
//     registry.saveTokenToEEPROM(token);
//     Serial.print(F("🆕 Generated new token: "));
//     Serial.println(token);
//   } else {
//     registry.setToken(token);
//     Serial.print(F("🔁 Reusing stored token: "));
//     Serial.println(token);
//   }

//   for (int attempt = 1; attempt <= 3; attempt++) {
//     Serial.print(F("📨 Attempt "));
//     Serial.print(attempt);
//     Serial.println(F(" to pair..."));

//     if (send.pairingRequest(token)) {
//       Serial.println(F("✅ Pairing request sent."));
//       TargetInfo target = receive.pairingResponse();
//       if (target.isValid()) {
//         target.token = token;
//         registry.setTargetInfo(target);
//         Serial.print(F("✅ Paired successfully with ID: "));
//         Serial.print(target.id);
//         Serial.print(F(" and color index: "));
//         Serial.println(target.colorIndex);
//         return true;
//       }
//       else {
//         Serial.println(F("❌ No valid pairing response received."));
//       }
//     }
//     else {
//       Serial.println(F("❌ Failed to send pairing request."));
//     }

//     delay(500);
//   }

//   Serial.println(F("❌ Pairing failed after multiple attempts."));
//   TargetInfo failed;
//   failed.id = 0xFF;
//   registry.setTargetInfo(failed);
//   return result;
// }

const bool Communication::verification() {
  TargetInfo target = verifyAssignedTarget();
  if (target.isValid()) {
    if (!send.verificationRequest(target.id)) {
      showStatus(statusRgbLed, STATUS_ERROR);
      return false;
    }

    if (!receive.verificationResponse(target.id)) {
      Serial.println(F("❌ Verification failed. Re-pairing..."));
      showStatus(statusRgbLed, STATUS_ERROR);
      pairingRequest();
      return false;
    }

    Serial.println(F("✅ Connection verified."));
    showStatus(statusRgbLed, STATUS_CONNECTED);
    return true;
  }
  return false;
}

HitResponsePacket Communication::hit() {
  TargetInfo target = verifyAssignedTarget();
  if (target.isValid()) {
    if (!send.hitRequest(target.id)) {
      return { OPCODE_SCORE_UPDATE, 0xFF, ScoreStatus::Error };
    }
    return receive.hitResponse();
  }
  return { OPCODE_SCORE_UPDATE, 0xFF, ScoreStatus::Error };
}

// const String Communication::entityColor(const byte* buffer) {
//   return receive.entityColor(buffer);
// }

const bool Communication::showTargetColor(const byte* buffer) {
  return receive.showTargetColorRequest(buffer);
}