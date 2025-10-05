#include "ScreenRenderer.h"
#include "ScreenManager.h"
#include "LcdDisplay.h"
#include "EncoderMode.h"
#include "ButtonLabels.h"
#include "screens/Screen.h"

ScreenRenderer::ScreenRenderer(LcdDisplay& d, ScreenManager& m, HubStateManager& state, GameModeRegistry& registry)
  : display(d), manager(m), hubState(state), gameModes(registry) {}

void ScreenRenderer::render() {
  // display.fadeOut();  // Optional: smooth transition between screens

  // Delegate to active screen
  Screen* screen = manager.getActive();
  if (screen) {
    screen->render(display);
  } else {
    display.showText("Screen not implemented");
  }

  // Button labels
  ButtonLabels labels = manager.getButtonLabels();
  display.showButtonLabels(labels.left, labels.right);

  // Encoder hint
  EncoderMode mode = manager.getEncoderMode();
  if (mode == EncoderMode::SelectItem || mode == EncoderMode::AdjustField) {
    display.printAligned("<>", display.getRowCount() - 1, "center");
  }
}