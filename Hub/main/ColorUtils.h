#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <Color.h>
#include <ColorPalette.h>

const uint8_t TargetColorIndexesPalette[] = {
  static_cast<uint8_t>(ColorPalette::getIndexByName("Fucsia")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Seafoam")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Red")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Lime")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Magenta")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Ultramarine")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Yellow")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Orange")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Blue")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Purple"))
};

const uint8_t EntityColorIndexesPalette[] = {
  static_cast<uint8_t>(ColorPalette::getIndexByName("Cyan")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Fucsia")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Green")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Orange")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Blue")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Red")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Ultramarine")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Yellow")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Purple")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Magenta")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Lime")),
  static_cast<uint8_t>(ColorPalette::getIndexByName("Seafoam"))
};

#endif