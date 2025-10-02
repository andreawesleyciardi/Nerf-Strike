// Screen.h
#ifndef SCREEN_H
#define SCREEN_H

#include "../DisplayManager.h"
#include <RotaryEncoder.h>
#include <Button.h>
#include "../ScreenTypes.h"

class ScreenManager;

class Screen {
public:
  virtual void render(DisplayManager& display) = 0;
  virtual void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) = 0;
  virtual ScreenType getType() const = 0;
  virtual EncoderMode getEncoderMode() const { return EncoderMode::None; }
  virtual ButtonLabels getButtonLabels() const { return {"", "", ""}; }
};

#endif