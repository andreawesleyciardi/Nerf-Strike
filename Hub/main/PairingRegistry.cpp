#include "PairingRegistry.h"
#include <string.h>

PairingRegistry::PairingRegistry() : nextID(1) {
  clearAll();
}

void PairingRegistry::clearAll() {
  for (int i = 0; i < MAX_TARGETS; i++) {
    entries[i].id = 0xFF;
    entries[i].token = 0;
    memset(entries[i].pipe, 0, sizeof(entries[i].pipe));
    entries[i].active = false;
  }
  nextID = 1;
}

int PairingRegistry::findEntryByToken(uint32_t token) {
  for (int i = 0; i < MAX_TARGETS; i++) {
    if (entries[i].active && entries[i].token == token) {
      return i;
    }
  }
  return -1;
}

int PairingRegistry::findEntryByID(uint8_t id) {
  for (int i = 0; i < MAX_TARGETS; i++) {
    if (entries[i].active && entries[i].id == id) {
      return i;
    }
  }
  return -1;
}

uint8_t PairingRegistry::assignID(uint32_t token) {
  int existing = findEntryByToken(token);
  if (existing != -1) {
    return entries[existing].id;  // ✅ Reuse existing ID
  }

  if (nextID > MAX_TARGETS) return 0xFF;

  for (int i = 0; i < MAX_TARGETS; i++) {
    if (!entries[i].active) {
      entries[i].id = nextID++;
      entries[i].token = token;
      entries[i].active = true;
      return entries[i].id;
    }
  }

  return 0xFF;
}

void PairingRegistry::storePipeForID(uint8_t id, const uint8_t* pipe) {
  int index = findEntryByID(id);
  if (index != -1) {
    memcpy(entries[index].pipe, pipe, 6);
  }
}

const uint8_t* PairingRegistry::getPipeForID(uint8_t id) {
  int index = findEntryByID(id);
  if (index != -1) {
    return entries[index].pipe;
  }
  return nullptr;
}

uint8_t PairingRegistry::getIDAt(uint8_t index) {
  if (index >= MAX_TARGETS || !entries[index].active) return 0xFF;
  return entries[index].id;
}

void PairingRegistry::setActive(uint8_t id, bool active) {
  for (auto& entry : entries) {
    if (entry.id == id) {
      entry.active = active;
      break;
    }
  }
}

uint8_t PairingRegistry::getPairedTargetCount() const {
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_TARGETS; ++i) {
    if (entries[i].active) {
      ++count;
    }
  }
  return count;
}

const uint8_t* PairingRegistry::getAllPairedTargetIds() const {
  static uint8_t pairedIds[MAX_TARGETS];
  uint8_t index = 0;

  for (uint8_t i = 0; i < MAX_TARGETS; ++i) {
    if (entries[i].active && index < MAX_TARGETS) {
      pairedIds[index++] = entries[i].id;
    }
  }

  return pairedIds;
}