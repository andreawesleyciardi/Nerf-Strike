#ifndef PAIRING_REGISTRY_H
#define PAIRING_REGISTRY_H

#include <Arduino.h>

#define MAX_TARGETS 10

class PairingRegistry {
public:
  PairingRegistry();

  uint8_t assignID(uint32_t token);
  uint8_t assignColor(uint8_t index);
  void storePipeForID(uint8_t id, const uint8_t* pipe);
  const uint8_t* getPipeForID(uint8_t id);
  uint8_t getIDAt(uint8_t index);
  void clearAll();
  void setActive(uint8_t id, bool active);
  uint8_t getPairedTargetCount() const;
  const uint8_t* getAllPairedTargetIds() const;

private:
  struct Target {
    uint8_t id;
    uint32_t token;
    uint8_t pipe[6];
    uint8_t colorIndex = 0xFF;
    bool active;
  };

  Target targets[MAX_TARGETS];
  uint8_t nextID;

  int findTargetByToken(uint32_t token);
  int findTargetByID(uint8_t id);
};

#endif
