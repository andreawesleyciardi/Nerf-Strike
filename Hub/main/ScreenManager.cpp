#include "ScreenManager.h"

// Include all screen headers
#include "screens/HomeScreen.h"
#include "screens/HelpScreen.h"
#include "screens/SettingsScreen.h"
#include "screens/PairingScreen.h"
#include "screens/TargetListScreen.h"
#include "screens/GameModeListScreen.h"
#include "screens/GameModeOptionsScreen.h"
#include "screens/GameModeDetailsScreen.h"
#include "screens/PlayingScreen.h"
#include "screens/ConfirmationScreen.h"
#include "screens/ErrorScreen.h"
#include "screens/WinLostScreen.h"

ScreenManager::ScreenManager(HubStateManager& hubState, GameModeRegistry& gameModes, PairingRegistry& registry, GameLogic& gameLogic)
  : hubState(hubState), gameModes(gameModes), registry(registry), gameLogic(gameLogic) {}

void ScreenManager::setup() {
  screens[(int)ScreenType::Home]             = new HomeScreen();
  screens[(int)ScreenType::Help]             = new HelpScreen();
  screens[(int)ScreenType::Settings]         = new SettingsScreen(hubState);
  screens[(int)ScreenType::Pairing]          = new PairingScreen(registry);
  screens[(int)ScreenType::TargetList]       = new TargetListScreen(registry);
  screens[(int)ScreenType::GameModeList]     = new GameModeListScreen(gameModes);
  screens[(int)ScreenType::GameModeOptions]  = new GameModeOptionsScreen(gameModes);
  screens[(int)ScreenType::GameModeDetails]  = new GameModeDetailsScreen(gameModes);
  screens[(int)ScreenType::Playing]          = new PlayingScreen(gameLogic, registry);
  screens[(int)ScreenType::Confirmation]     = new ConfirmationScreen(hubState);
  screens[(int)ScreenType::Error]            = new ErrorScreen(hubState);
  screens[(int)ScreenType::WinLost]          = new WinLostScreen(hubState, registry, gameLogic);

  replace(ScreenType::Home);  // Start with Home screen
}

void ScreenManager::replace(ScreenType screen) {
  activeScreen = screen;
  updateContext(screen);
}

ScreenType ScreenManager::current() const {
  return activeScreen;
}

Screen* ScreenManager::getActive() const {
  return screens[(int)activeScreen];
}

EncoderMode ScreenManager::getEncoderMode() const {
  return encoderMode;
}

ButtonLabels ScreenManager::getButtonLabels() const {
  return labels;
}

void ScreenManager::updateContext(ScreenType screen) {
  Screen* active = getActive();
  if (active) {
    encoderMode = active->getEncoderMode();
    labels = active->getButtonLabels();
  } else {
    encoderMode = EncoderMode::None;
    labels = {"", "", ""};
  }
}