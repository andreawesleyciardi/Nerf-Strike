#ifndef SCREEN_TYPES_H
#define SCREEN_TYPES_H

#include <Arduino.h>

enum class ScreenType {
  None,
  Splash,
  Home,
  Help,
  Settings,
  Pairing,
  TargetList,
  GameModeList,
  GameModeOptions,
  Playing,
  Confirmation,
  Error,
  WinLost,
  Entities
};

#endif