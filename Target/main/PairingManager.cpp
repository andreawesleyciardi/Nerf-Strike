#include "PairingManager.h"
#include <EEPROM.h>
#include "TargetConfig.h"

PairingManager::PairingManager(WirelessManager &wm) : wireless(wm) {}

bool PairingManager::isPaired() {
  return EEPROM.read(0) == 1;
}

uint8_t PairingManager::getAssignedID() {
  return EEPROM.read(1);
}

void PairingManager::storeAssignedID(uint8_t id) {
  EEPROM.write(0, 1);
  EEPROM.write(1, id);
  Serial.print("💾 Stored assigned ID in EEPROM: ");
  Serial.println(id);
}

uint32_t PairingManager::generateToken() {
  uint32_t token = (uint32_t)random(100000, 999999);
  Serial.print("🔐 Generated token: ");
  Serial.println(token);
  return token;
}

bool PairingManager::pair() {
  Serial.println("🔁 Starting pairing process...");
  uint32_t token = generateToken();

  const int maxRetries = 3;
  for (int attempt = 1; attempt <= maxRetries; attempt++) {
    Serial.print("📨 Attempt ");
    Serial.print(attempt);
    Serial.println(" to pair...");

    wireless.sendPairingRequest(token);

    uint8_t newID;
    if (wireless.receivePairingResponse(newID)) {
      storeAssignedID(newID);
      wireless.switchToTargetPipe(newID);

      Serial.print("✅ Paired successfully with ID: ");
      Serial.println(newID);

      digitalWrite(redLEDPin, LOW);
      digitalWrite(blueLEDPin, LOW);
      digitalWrite(greenLEDPin, HIGH);
      return true;
    }

    delay(500);
  }

  Serial.println("❌ Pairing failed after multiple attempts.");
  digitalWrite(redLEDPin, HIGH);
  digitalWrite(greenLEDPin, LOW);
  digitalWrite(blueLEDPin, LOW);
  return false;
}

bool PairingManager::verifyPairing(uint8_t id) {
  Serial.print("🔍 Verifying stored ID ");
  Serial.print(id);
  Serial.println(" with hub...");

  const int maxRetries = 2;
  for (int attempt = 1; attempt <= maxRetries; attempt++) {
    Serial.print("🔁 Verification attempt ");
    Serial.println(attempt);

    wireless.sendVerificationRequest(id);
    if (wireless.waitForVerificationAck(id)) {
      wireless.switchToTargetPipe(id);
      Serial.println("✅ ID verified with hub.");
      return true;
    }

    delay(500);
  }

  Serial.println("❌ ID not recognized by hub after retries.");
  return false;
}

uint8_t PairingManager::readFlag() {
  return EEPROM.read(0);
}

uint8_t PairingManager::readId() {
  return EEPROM.read(1);
}

void PairingManager::clear() {
  EEPROM.write(0, 0); // Clear pairing flag
  EEPROM.write(1, 0); // Clear assigned ID
}
