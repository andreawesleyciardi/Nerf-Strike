#ifndef PAIRING_REGISTRY_H
#define PAIRING_REGISTRY_H

#include <Arduino.h>
#include <Target.h>

#define MAX_TARGETS 10

class PairingRegistry {
public:
  PairingRegistry();

  TargetInfo setTarget(uint32_t token);  // Assigns or retrieves target info by token
  void storeTargetInfo(const TargetInfo& info);

  bool hasToken(uint32_t token) const;
  TargetInfo getInfoByToken(uint32_t token) const;

  const uint8_t* getPipeForID(uint8_t id);
  uint8_t getIDAt(uint8_t index);
  void clearAll();
  void setEnabled(uint8_t id, bool enabled);
  const uint8_t* getEnabledTargetIds() const;
  uint8_t getPairedTargetCount() const;
  const uint8_t* getAllPairedTargetIds() const;
  TargetInfo getTargetByID(uint8_t id) const;
  void logTargets() const;

private:
  TargetInfo targets[MAX_TARGETS];
  uint8_t nextID;

  int findTargetByToken(uint32_t token);
  int findTargetByID(uint8_t id) const;
};

#endif