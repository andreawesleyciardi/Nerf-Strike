#include "ScreenManager.h"

// Include all screen headers
#include "screens/SplashScreen.h"
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

ScreenManager::ScreenManager(LcdDisplay& display, HubStateManager& hubState, GameModeRegistry& gameModes, PairingRegistry& registry, GameLogic& gameLogic)
  : display(display), hubState(hubState), gameModes(gameModes), registry(registry), gameLogic(gameLogic) {}

void ScreenManager::setup() {
  screens[(int)ScreenType::Splash]           = new SplashScreen(display);
  screens[(int)ScreenType::Home]             = new HomeScreen(display, registry);
  screens[(int)ScreenType::Help]             = new HelpScreen(display);
  screens[(int)ScreenType::Settings]         = new SettingsScreen(display, hubState);
  screens[(int)ScreenType::Pairing]          = new PairingScreen(display, registry);
  screens[(int)ScreenType::TargetList]       = new TargetListScreen(display, registry);
  screens[(int)ScreenType::GameModeList]     = new GameModeListScreen(display, gameModes);
  screens[(int)ScreenType::GameModeOptions]  = new GameModeOptionsScreen(display, gameModes);
  screens[(int)ScreenType::GameModeDetails]  = new GameModeDetailsScreen(display, gameModes);
  screens[(int)ScreenType::Playing]          = new PlayingScreen(display, gameLogic, registry);
  screens[(int)ScreenType::Confirmation]     = new ConfirmationScreen(display, hubState);
  screens[(int)ScreenType::Error]            = new ErrorScreen(display, hubState);
  screens[(int)ScreenType::WinLost]          = new WinLostScreen(display, hubState, registry, gameLogic);

  replace(ScreenType::Splash);  // Start with Home screen
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