#include "PairingRegistry.h"
#include <string.h>

PairingRegistry::PairingRegistry() : nextID(1) {
  clearAll();
}

void PairingRegistry::clearAll() {
  for (int i = 0; i < MAX_TARGETS; i++) {
    targets[i].id = 0xFF;
    targets[i].token = 0;
    memset(targets[i].pipe, 0, sizeof(targets[i].pipe));
    targets[i].enabled = true;
  }
  nextID = 1;
}

int PairingRegistry::findTargetByToken(uint32_t token) {
  for (int i = 0; i < MAX_TARGETS; i++) {
    if (targets[i].token == token) {
      return i;
    }
  }
  return -1;
}

int PairingRegistry::findTargetByID(uint8_t id) const {
  for (int i = 0; i < MAX_TARGETS; i++) {
    if (targets[i].id == id) {
      return i;
    }
  }
  return -1;
}

TargetInfo PairingRegistry::setTarget(uint32_t token) {
  int existingIndex = findTargetByToken(token);
  if (existingIndex != -1) {
    return targets[existingIndex];
  }

  if (nextID > MAX_TARGETS) return TargetInfo();

  for (int i = 0; i < MAX_TARGETS; i++) {
    if (targets[i].id == 0xFF) {
      targets[i].id = nextID++;
      targets[i].token = token;
      targets[i].colorIndex = i % 10;
      return targets[i];
    }
  }

  return TargetInfo();
}

TargetInfo PairingRegistry::getTargetByID(uint8_t id) const {
  int index = findTargetByID(id);
  return (index != -1) ? targets[index] : TargetInfo();
}

void PairingRegistry::storeTargetInfo(const TargetInfo& info) {
  int index = findTargetByID(info.id);
  if (index != -1) {
    targets[index] = info;
  }
}

const uint8_t* PairingRegistry::getPipeForID(uint8_t id) {
  int index = findTargetByID(id);
  if (index != -1) {
    return targets[index].pipe;
  }
  return nullptr;
}

uint8_t PairingRegistry::getIDAt(uint8_t index) {
  if (index >= MAX_TARGETS || targets[index].id == 0xFF) return 0xFF;
  return targets[index].id;
}

void PairingRegistry::setEnabled(uint8_t id, bool enabled) {
  int index = findTargetByID(id);
  if (index != -1) {
    targets[index].enabled = enabled;
  }
}

const uint8_t* PairingRegistry::getEnabledTargetIds() const {
  static uint8_t enabledIds[MAX_TARGETS];
  uint8_t index = 0;

  for (uint8_t i = 0; i < MAX_TARGETS; ++i) {
    if (targets[i].id != 0xFF && targets[i].enabled && index < MAX_TARGETS) {
      enabledIds[index++] = targets[i].id;
    }
  }

  return enabledIds;
}

uint8_t PairingRegistry::getPairedTargetCount() const {
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_TARGETS; ++i) {
    if (targets[i].id != 0xFF) {
      ++count;
    }
  }
  return count;
}

const uint8_t* PairingRegistry::getAllPairedTargetIds() const {
  static uint8_t pairedIds[MAX_TARGETS];
  uint8_t index = 0;

  for (uint8_t i = 0; i < MAX_TARGETS; ++i) {
    if (targets[i].id != 0xFF && index < MAX_TARGETS) {
      pairedIds[index++] = targets[i].id;
    }
  }

  return pairedIds;
}

void PairingRegistry::logTargets() const {
  Serial.println(F("📋 Target Registry:"));
  for (int i = 0; i < MAX_TARGETS; i++) {
    if (targets[i].id != 0xFF) {
      Serial.print(F("  ID: "));
      Serial.print(targets[i].id);
      Serial.print(F(" | Token: "));
      Serial.print(targets[i].token);
      Serial.print(F(" | Pipe: "));
      Serial.write(targets[i].pipe, 5);
      Serial.print(F(" | Color: "));
      Serial.println(targets[i].colorIndex);
    }
  }
}