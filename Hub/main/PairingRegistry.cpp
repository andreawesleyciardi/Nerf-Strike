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
    targets[i].active = false;
  }
  nextID = 1;
}

int PairingRegistry::findTargetByToken(uint32_t token) {
  for (int i = 0; i < MAX_TARGETS; i++) {
    if (targets[i].active && targets[i].token == token) {
      return i;
    }
  }
  return -1;
}

int PairingRegistry::findTargetByID(uint8_t id) {
  for (int i = 0; i < MAX_TARGETS; i++) {
    if (targets[i].active && targets[i].id == id) {
      return i;
    }
  }
  return -1;
}

uint8_t PairingRegistry::assignID(uint32_t token) {
  int existing = findTargetByToken(token);
  if (existing != -1) {
    return targets[existing].id;  // ✅ Reuse existing ID
  }

  if (nextID > MAX_TARGETS) return 0xFF;

  for (int i = 0; i < MAX_TARGETS; i++) {
    if (!targets[i].active) {
      targets[i].id = nextID++;
      targets[i].token = token;
      targets[i].active = true;
      return targets[i].id;
    }
  }

  return 0xFF;
}

void PairingRegistry::storePipeForID(uint8_t id, const uint8_t* pipe) {
  int index = findTargetByID(id);
  if (index != -1) {
    memcpy(targets[index].pipe, pipe, 6);
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
  if (index >= MAX_TARGETS || !targets[index].active) return 0xFF;
  return targets[index].id;
}

void PairingRegistry::setActive(uint8_t id, bool active) {
  for (auto& target : targets) {
    if (target.id == id) {
      target.active = active;
      break;
    }
  }
}

uint8_t PairingRegistry::getPairedTargetCount() const {
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_TARGETS; ++i) {
    if (targets[i].active) {
      ++count;
    }
  }
  return count;
}

const uint8_t* PairingRegistry::getAllPairedTargetIds() const {
  static uint8_t pairedIds[MAX_TARGETS];
  uint8_t index = 0;

  for (uint8_t i = 0; i < MAX_TARGETS; ++i) {
    if (targets[i].active && index < MAX_TARGETS) {
      pairedIds[index++] = targets[i].id;
    }
  }

  return pairedIds;
}