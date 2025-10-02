#ifndef TARGET_TYPE_MANAGER_H
#define TARGET_TYPE_MANAGER_H

#include <Arduino.h>
#include <TargetType.h>

#define EEPROM_TARGET_TYPE_ADDR 0  // EEPROM address for storing allowed target type

class TargetTypeManager {
public:
  TargetTypeManager();

  void setAllowedType(TargetType type);
  TargetType getAllowedType() const;

  bool isCompatible(TargetType incoming) const;

  void loadFromEEPROM();
  void saveToEEPROM();

private:
  TargetType allowedType;
};

#endif