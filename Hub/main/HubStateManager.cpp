#include "HubStateManager.h"

void HubStateManager::begin() {
  currentMode = MODE_IDLE;
  selectedTargetID = 0xFF;
  menuIndex = 0;
}

void HubStateManager::setGameMode(GameMode mode) {
  currentMode = mode;
}

GameMode HubStateManager::getGameMode() {
  return currentMode;
}

void HubStateManager::setSelectedTarget(uint8_t id) {
  selectedTargetID = id;
}

uint8_t HubStateManager::getSelectedTarget() {
  return selectedTargetID;
}

void HubStateManager::nextMenuItem() {
  menuIndex++;
}

void HubStateManager::previousMenuItem() {
  if (menuIndex > 0) menuIndex--;
}

uint8_t HubStateManager::getCurrentMenuIndex() {
  return menuIndex;
}