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
    display.showText("Screen not implemented");
    lastRenderedHash = "error";
    lastRenderedScreen = currentScreen;
    return;
  }

  String currentHash = screen->getHash();

  // ✅ Skip rendering if screen and content haven't changed
  if (!forceRender && currentScreen == lastRenderedScreen && currentHash == lastRenderedHash) {
    return;
  }

  forceRender = false;  // Reset flag after use

  display.clear();
  screen->render();
  lastRenderedHash = currentHash;
  lastRenderedScreen = currentScreen;

  // ✅ Button labels
  ButtonLabels labels = manager.getButtonLabels();
  display.showButtonLabels(labels);

  // ✅ Encoder hint
  EncoderMode mode = manager.getEncoderMode();
  if (mode == EncoderMode::SelectItem || mode == EncoderMode::AdjustField) {
    display.printAligned("<>", display.getRowCount() - 1, "center");
  }
}

void ScreenRenderer::requestRefresh() {
  forceRender = true;
}