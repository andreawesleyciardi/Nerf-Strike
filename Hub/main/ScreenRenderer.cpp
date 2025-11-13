#include "ScreenRenderer.h"
#include "ScreenManager.h"
#include "EncoderMode.h"
#include "ButtonLabels.h"
#include "screens/Screen.h"

ScreenRenderer::ScreenRenderer(LcdDisplay& d, ScreenManager& m, HubStateManager& state, GameModeRegistry& registry)
  : display(d), manager(m), hubState(state), gameModes(registry),
    lastRenderedScreen(ScreenType::None), lastRenderedHash(""), forceRender(false) {}

void ScreenRenderer::render() {
  ScreenType currentScreen = manager.current();
  Screen* screen = manager.getActive();

  if (!screen) {
    display.clear();
    display.showText("Screen not implemented");
    lastRenderedHash = "error";
    lastRenderedScreen = currentScreen;
    return;
  }

  String currentHash = screen->getHash();

  // 🔍 Detect screen change or forced refresh
  bool screenChanged = (currentScreen != lastRenderedScreen);
  bool hashChanged = (currentHash != lastRenderedHash);

  if (!forceRender && !screenChanged && !hashChanged) {
    return;  // ✅ Skip rendering if nothing changed
  }

  // 🧹 Reset render flag and update tracking
  forceRender = false;
  lastRenderedHash = currentHash;
  lastRenderedScreen = currentScreen;

  // 🧼 Full redraw
  display.clear();
  screen->render();

  // 🎯 Button labels
  ButtonLabels labels = screen->getButtonLabels();
  display.showButtonLabels(labels);

  // 🎛 Encoder hint
  EncoderMode mode = manager.getEncoderMode();
  if (mode == EncoderMode::SelectItem || mode == EncoderMode::AdjustField) {
    // display.printAligned("<>", display.getRowCount() - 1, "center");
    display.setCursor(9, 3);
    display.write(1);
    display.write(2);
  }

  // 🧪 Debug trace (optional)
  // Serial.print(F("Rendered screen: "));
  // Serial.println((int)currentScreen);
  // Serial.print(F("Hash: "));
  // Serial.println(currentHash);
}

void ScreenRenderer::requestRefresh() {
  forceRender = true;
}