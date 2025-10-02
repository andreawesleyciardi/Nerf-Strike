#include "ScreenRenderer.h"

ScreenRenderer::ScreenRenderer(DisplayManager& d, ScreenManager& m, HubStateManager& state, GameModeRegistry& registry)
  : display(d), manager(m), hubState(state), gameModes(registry) {}

void ScreenRenderer::render() {
  display.fadeOut();  // Optional: smooth transition between screens

  switch (manager.current()) {
    case ScreenID::Home: renderHome(); break;
    case ScreenID::Help: renderHelp(); break;
    case ScreenID::Pairing: renderPairing(); break;
    case ScreenID::GameModeList: renderGameModeList(); break;
    default: display.showText("Screen not implemented");
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

void ScreenRenderer::renderHome() {
  display.fadeIn("Welcome", 0);
}

void ScreenRenderer::renderHelp() {
  display.showText("Commands:\nlist, test, clear", false);
}

void ScreenRenderer::renderPairing() {
  static uint8_t dotCount = 0;
  String dots = "";
  for (int i = 0; i < dotCount % 4; i++) dots += ".";
  display.printAligned("Pairing" + dots, 0, "center");
  dotCount++;
}

void ScreenRenderer::renderGameModeList() {
  uint8_t index = hubState.getCurrentMenuIndex();
  const GameMode& mode = gameModes.getMode(index);
  String label = mode.getName();
  display.showMenuItem(label, index, gameModes.getModeCount());

  // Optional: show description based on number of players
  uint8_t pairedTargets = 2;  // Replace with registry.countPaired() if available
  String desc = mode.getDescription().getTextForPlayers(pairedTargets);
  display.printAligned(desc, 1, "center");
}