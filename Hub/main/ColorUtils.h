#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <Color.h>

const Color EntityColorSequence[] = {
  Color(0, 255, 255, "Cyan"),
  Color(255, 0, 128, "Fucsia"),
  Color(0, 255, 0, "Green"),
  Color(255, 128, 0, "Orange"),
  Color(0, 0, 255, "Blue"),
  Color(255, 0, 0, "Red"),
  Color(0, 128, 255, "Ultramarine"),
  Color(255, 255, 0, "Yellow"),
  Color(128, 0, 255, "Purple"),
  Color(255, 0, 255, "Magenta"),
  Color(128, 255, 0, "Lime"),
  Color(0, 255, 128, "Seafoam")
};

inline Color getColorForIndex(uint8_t index) {
  uint8_t colorIndex = index % (sizeof(EntityColorSequence) / sizeof(Color));
  return EntityColorSequence[colorIndex];
}

#endif