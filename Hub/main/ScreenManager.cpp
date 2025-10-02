#include "ScreenManager.h"

void ScreenManager::setup() {
  stackSize = 0;
  push(ScreenID::Home);
}

void ScreenManager::push(ScreenID screen) {
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

void ScreenManager::replace(ScreenID screen) {
  if (stackSize > 0) {
    stack[stackSize - 1] = screen;
    updateContext(screen);
  } else {
    push(screen);
  }
}

ScreenID ScreenManager::current() {
  return stackSize > 0 ? stack[stackSize - 1] : ScreenID::Home;
}

EncoderMode ScreenManager::getEncoderMode() {
  return encoderMode;
}

ButtonLabels ScreenManager::getButtonLabels() {
  return labels;
}

void ScreenManager::updateContext(ScreenID screen) {
  switch (screen) {
    case ScreenID::Home:
      encoderMode = EncoderMode::None;
      labels = {"Help", "", "Start"};
      break;
    case ScreenID::Help:
      encoderMode = EncoderMode::None;
      labels = {"", "", "Home"};
      break;
    case ScreenID::Pairing:
      encoderMode = EncoderMode::None;
      labels = {"Targets", "", "Play"};
      break;
    case ScreenID::TargetList:
      encoderMode = EncoderMode::ScrollList;
      labels = {"Re-Pair", "<>", "Play"};
      break;
    case ScreenID::GameModeList:
      encoderMode = EncoderMode::ScrollList;
      labels = {"Exit", "<>", "Select"};
      break;
    case ScreenID::GameModeOptions:
      encoderMode = EncoderMode::AdjustField;
      labels = {"Back", "<>", "Select"};
      break;
    case ScreenID::GameModeDetails:
      encoderMode = EncoderMode::None;
      labels = {"Back", "", "Start"};
      break;
    case ScreenID::Playing:
      encoderMode = EncoderMode::None;
      labels = {"Stop", "<>", "Restart"};
      break;
    case ScreenID::Confirmation:
      encoderMode = EncoderMode::None;
      labels = {"Cancel", "", "Confirm"};
      break;
    case ScreenID::Error:
      encoderMode = EncoderMode::None;
      labels = {"Back", "", "Retry"};
      break;
    case ScreenID::WinLost:
      encoderMode = EncoderMode::None;
      labels = {"Home", "", "Restart"};
      break;
  }
}