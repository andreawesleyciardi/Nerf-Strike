#ifndef SCREEN_TYPES_H
#define SCREEN_TYPES_H

#include <Arduino.h>

enum class ScreenType {
  Home,
  Help,
  Settings,
  Pairing,
  TargetList,
  GameModeList,
  GameModeOptions,
  GameModeDetails,
  Playing,
  Confirmation,
  Error,
  WinLost
};

#endif