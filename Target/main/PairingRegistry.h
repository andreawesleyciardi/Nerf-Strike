#ifndef PAIRING_REGISTRY_H
#define PAIRING_REGISTRY_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Target.h>

#include "WirelessTarget.h"

class PairingRegistry {
public:
  PairingRegistry(RF24& radio);

  void setTargetInfo(const TargetInfo& info);
  TargetInfo getTargetInfo() const;

  void setToken(uint32_t token);
  void resetToken();

  uint32_t loadTokenFromEEPROM();
  void saveTokenToEEPROM(uint32_t token);
  void saveTargetInfoToEEPROM(const TargetInfo& info);
  TargetInfo loadTargetInfoFromEEPROM();
  void resetTargetInfoEEPROM();

  void switchToPairingPipe();
  void switchToTargetPipe(uint8_t id);
  const uint8_t* getTargetPipe() const;

private:
  RF24& radio;
  TargetInfo target;
};

#endif