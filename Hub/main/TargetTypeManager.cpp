#include "TargetTypeManager.h"
#include <EEPROM.h>

TargetTypeManager::TargetTypeManager() : allowedType(TargetType::StrikeLite) {}

void TargetTypeManager::setAllowedType(TargetType type) {
  allowedType = type;
}

TargetType TargetTypeManager::getAllowedType() const {
  return allowedType;
}

bool TargetTypeManager::isCompatible(TargetType incoming) const {
  return incoming == allowedType;
}

void TargetTypeManager::loadFromEEPROM() {
  uint8_t raw = EEPROM.read(EEPROM_TARGET_TYPE_ADDR);
  if (raw >= 0x01 && raw <= 0x03) {
    allowedType = static_cast<TargetType>(raw);
  } else {
    allowedType = TargetType::StrikeLite;  // Default fallback
  }
}

void TargetTypeManager::saveToEEPROM() {
  EEPROM.write(EEPROM_TARGET_TYPE_ADDR, static_cast<uint8_t>(allowedType));
}