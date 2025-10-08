#ifndef HUB_STATE_MANAGER_H
#define HUB_STATE_MANAGER_H

#include <stdint.h>

enum HubGameState {
  MODE_IDLE,
  MODE_PAIRING,
  MODE_RUNNING,
  MODE_PAUSED,
  MODE_FINISHED
};

class HubStateManager {
public:
  void begin();
  void setGameMode(HubGameState mode);
  HubGameState getGameMode();

  void setSelectedTarget(uint8_t id);
  uint8_t getSelectedTarget();

  void nextMenuItem();
  void previousMenuItem();
  uint8_t getCurrentMenuIndex();

  // const String getLastErrorMessage();

private:
  HubGameState currentMode;
  uint8_t selectedTargetID;
  uint8_t menuIndex;
};

#endif