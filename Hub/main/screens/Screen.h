#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <RotaryEncoder.h>
#include <Button.h>

#include "../LcdDisplay.h"
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

  // Called every frame to render the screen
  virtual void render(LcdDisplay& display) = 0;

  // Called every frame to handle input
  virtual void handleInput(RotaryEncoder& encoder, Button& left, Button& right) = 0;

  // Returns a requested screen transition (if any)
  virtual ScreenRequest getRequestedScreen() const {
    return request;
  }

  // Metadata and UI hints
  virtual ScreenType getType() const = 0;

  virtual EncoderMode getEncoderMode() const {
    return EncoderMode::None;
  }

  virtual ButtonLabels getButtonLabels() const {
    return {"", "", ""};
  }

  virtual String getHash() const {
    return "";
  }

protected:
  ScreenRequest request;
};

#endif