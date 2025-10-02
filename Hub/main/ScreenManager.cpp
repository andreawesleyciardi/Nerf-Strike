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

void ScreenManager::setup() {
  // Register all screens
  screens[(int)ScreenType::Home] = new HomeScreen();
  screens[(int)ScreenType::Help] = new HelpScreen();
  screens[(int)ScreenType::Settings] = new SettingsScreen();
  screens[(int)ScreenType::Pairing] = new PairingScreen();
  screens[(int)ScreenType::TargetList] = new TargetListScreen();
  screens[(int)ScreenType::GameModeList] = new GameModeListScreen();
  screens[(int)ScreenType::GameModeOptions] = new GameModeOptionsScreen();
  screens[(int)ScreenType::GameModeDetails] = new GameModeDetailsScreen();
  screens[(int)ScreenType::Playing] = new PlayingScreen();
  screens[(int)ScreenType::Confirmation] = new ConfirmationScreen();
  screens[(int)ScreenType::Error] = new ErrorScreen();
  screens[(int)ScreenType::WinLost] = new WinLostScreen();

  // Start with Home screen
  stackSize = 1;
  stack[0] = ScreenType::Home;
  updateContext(ScreenType::Home);
}

void ScreenManager::push(ScreenType screen) {
  if (stackSize < maxStackDepth) {
    stack[stackSize++] = screen;
    updateContext(screen);
  }
}

void ScreenManager::pop() {
  if (stackSize > 1) {
    stackSize--;
    updateContext(stack[stackSize - 1]);
  }
}

void ScreenManager::replace(ScreenType screen) {
  if (stackSize > 0) {
    stack[stackSize - 1] = screen;
    updateContext(screen);
  }
}

ScreenType ScreenManager::current() {
  return stackSize > 0 ? stack[stackSize - 1] : ScreenType::Home;
}

Screen* ScreenManager::getActive() {
  ScreenType type = current();
  return screens[(int)type];
}

EncoderMode ScreenManager::getEncoderMode() {
  return encoderMode;
}

ButtonLabels ScreenManager::getButtonLabels() {
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