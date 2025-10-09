#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class SplashScreen : public Screen {
public:
  SplashScreen(LcdDisplay& display, unsigned long durationMs = 6000)
    : display(display), duration(durationMs), startTime(0), initialized(false) {}

  void render() override {
    Serial.println(F(" dSplashScreen rendered"));
    if (!initialized) {
      startTime = millis();
      initialized = true;
    }

    display.clear();
    display.showLine(1, "Welcome to the Hub", "center");
    display.showLine(2, "Loading...", "center");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (millis() - startTime >= duration) {
      request = ScreenRequest::to(ScreenType::Home);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::None;
  }

  ButtonLabels getButtonLabels() const override {
    return {"", "", ""};
  }

  ScreenType getType() const override {
    return ScreenType::Splash;
  }

  String getHash() const override {
    return "Splash";
  }

private:
  LcdDisplay& display;
  unsigned long duration;
  unsigned long startTime;
  bool initialized;
};

#endif