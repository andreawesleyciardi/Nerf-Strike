#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <RotaryEncoder.h>
#include <Button.h>

// #include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"

// Represents a screen transition request
struct ScreenRequest {
  bool hasRequest;
  ScreenType target;

  ScreenRequest() : hasRequest(false), target(ScreenType::Home) {}

  ScreenRequest(bool request, ScreenType target)
    : hasRequest(request), target(target) {}

  static ScreenRequest none() {
    return ScreenRequest(false, ScreenType::Home);
  }

  static ScreenRequest to(ScreenType target) {
    return ScreenRequest(true, target);
  }
};

class Screen {
public:
  virtual ~Screen() {}

  virtual void render() = 0;
  virtual void handleInput(RotaryEncoder& encoder, Button& left, Button& right) = 0;
  virtual void onEnter() {}
  virtual void onExit() {}

  virtual ScreenRequest getRequestedScreen() const {
    return request;
  }

  void clearRequest() { request = ScreenRequest::none(); }  // ✅ NEW

  virtual ScreenType getType() const = 0;
  virtual EncoderMode getEncoderMode() const { return EncoderMode::None; }
  virtual ButtonLabels getButtonLabels() const { return {"", "", ""}; }
  virtual String getHash() const { return ""; }
  virtual void loop() {}

protected:
  ScreenRequest request;
};

#endif