#ifndef SCREEN_TYPES_H
#define SCREEN_TYPES_H

#include <Arduino.h>

enum class ScreenID {
  Home,
  Help,
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

enum class EncoderMode {
  ScrollList,
  SelectItem,
  AdjustField,
  None
};

struct ButtonLabels {
  String left;
  String center;  // e.g. "<>" when encoder is active
  String right;
};

#endif