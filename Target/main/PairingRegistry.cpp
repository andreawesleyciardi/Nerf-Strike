#include "PairingRegistry.h"

#define EEPROM_TOKEN_ADDR 0  // Starting address to store token

PairingRegistry::PairingRegistry(RF24& radio) : radio(radio) {
  target.id = 0xFF;
  target.token = 0;
  memset(target.pipe, 0, sizeof(target.pipe));
  target.colorIndex = 0xFF;
  // target.active = false;
  target.enabled = true;
}

void PairingRegistry::setTargetInfo(const TargetInfo& info) {
  target = info;
  Serial.println();
  
  if (target.id != 0xFF) {
    Serial.println(F("🧍 Setted Target Infos."));
    Serial.println();
    Serial.println(F("🔀 setTargetInfo: Switching to assigned pipe..."));
    switchToTargetPipe(target.id);
  }
  else {
    Serial.println(F("❌ Target Infos not setted."));
  }
}

TargetInfo PairingRegistry::getTargetInfo() const {
  return target;
}

// void PairingRegistry::setAssignedID(uint8_t id) {
//   target.id = id;
//   if (id != 0xFF) {
//     Serial.println("Switched to Target Pipe");
//     switchToTargetPipe(id);
//   }
// }

// void PairingRegistry::setAssignedColor(uint8_t colorIndex) {
//   target.colorIndex = colorIndex;
// }

void PairingRegistry::setToken(uint32_t token) {
  target.token = token;
}

uint32_t PairingRegistry::loadTokenFromEEPROM() {
  uint32_t storedToken;
  EEPROM.get(EEPROM_TOKEN_ADDR, storedToken);
  return storedToken;
}

void PairingRegistry::saveTokenToEEPROM(uint32_t token) {
  target.token = token;
  EEPROM.put(EEPROM_TOKEN_ADDR, token);
}

void PairingRegistry::resetToken() {
  uint32_t blank = 0xFFFFFFFF;
  EEPROM.put(EEPROM_TOKEN_ADDR, blank);
  target.token = blank;
  Serial.println(F("🧹 Token cleared from EEPROM."));
}

void PairingRegistry::switchToPairingPipe() {
  radio.stopListening();
  radio.openReadingPipe(1, pairingPipe);
  radio.startListening();

  Serial.println(F("🔀 Switched to pairing pipe."));
}

void PairingRegistry::switchToPairingPollPipe() {
  radio.stopListening();
  radio.openReadingPipe(1, pairingPollPipe);
  radio.startListening();

  Serial.println(F("🔀 Switched to pairing poll pipe."));
}

void PairingRegistry::switchToTargetPipe(uint8_t id) {
  sprintf((char*)target.pipe, "TGT%d", id);
  radio.stopListening();
  radio.openReadingPipe(1, target.pipe);
  radio.startListening();

  Serial.print(F("🔀 Switched to target pipe: "));
  Serial.println((char*)target.pipe);
}

const uint8_t* PairingRegistry::getTargetPipe() const {
  Serial.print(F("📡 getTargetPipe returns: "));
  for (int i = 0; i < 6; i++) {
    Serial.print((char)target.pipe[i]);
  }
  Serial.println();
  return target.pipe;
}