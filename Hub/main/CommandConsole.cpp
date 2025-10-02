#include "CommandConsole.h"
#include "HubConfig.h"

CommandConsole::CommandConsole(PairingRegistry& registry, WirelessHub& wireless, RotaryEncoder& encoder, Button& leftButton, Button& rightButton)
  : registry(registry), wireless(wireless), encoder(encoder), leftButton(leftButton), rightButton(rightButton) {}

void CommandConsole::processSerial() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "list") {
      dumpRegistry();
    } else if (cmd == "test") {
      testTargets();
    } else if (cmd == "clear") {
      clearRegistry();
    } else {
      Serial.println("❓ Unknown command. Try 'list', 'test', or 'clear'.");
    }
  }
}

void CommandConsole::processInput() {
  int dir = encoder.getDirection();
  if (dir != 0) {
    menuIndex -= dir;  // Encoder up = decrease index
    if (menuIndex < 0) menuIndex = 0;
    if (menuIndex >= maxMenuItems) menuIndex = maxMenuItems - 1;
    showMenu();
  }

  if (encoder.wasPressed()) {
    selectMenuItem(menuIndex);
  }

  if (leftButton.wasPressed()) {
    cancelMenu();
  }

  if (rightButton.wasPressed()) {
    confirmMenu();
  }
}

void CommandConsole::showMenu() {
  Serial.print(F("📜 Menu item: "));
  switch (menuIndex) {
    case 0: Serial.println(F("List Targets")); break;
    case 1: Serial.println(F("Test Targets")); break;
    case 2: Serial.println(F("Clear Registry")); break;
    default: Serial.println(F("Unknown")); break;
  }
}

void CommandConsole::selectMenuItem(int index) {
  Serial.print(F("✅ Selected: "));
  switch (index) {
    case 0: Serial.println(F("List Targets")); dumpRegistry(); break;
    case 1: Serial.println(F("Test Targets")); testTargets(); break;
    case 2: Serial.println(F("Clear Registry")); clearRegistry(); break;
    default: Serial.println(F("Unknown")); break;
  }
}

void CommandConsole::cancelMenu() {
  Serial.println(F("↩️ Cancel/back triggered"));
  // Optional: reset menuIndex or exit submenu
}

void CommandConsole::confirmMenu() {
  Serial.println(F("➡️ Confirm/next triggered"));
  selectMenuItem(menuIndex);
}

void CommandConsole::dumpRegistry() {
  Serial.println("📋 Paired Targets:");
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      Serial.print("🆔 ID: ");
      Serial.print(id);
      Serial.print(" | Pipe: ");
      for (int j = 0; j < 5; j++) Serial.print((char)pipe[j]);
      Serial.println();
    }
  }
}

void CommandConsole::testTargets() {
  Serial.println("🧪 Entering test mode: blinking all targets...");
  byte msg[2] = {0x05, 0x00};  // Blink command
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      Serial.print("🔔 Blinking ID: ");
      Serial.println(id);
      wireless.sendToTargetPipe(id, pipe, msg, sizeof(msg));
      delay(1000);
    }
  }
  Serial.println("✅ Test mode complete.");
}

void CommandConsole::clearRegistry() {
  Serial.println("🧹 Clearing all paired targets...");
  registry.clearAll();
  Serial.println("✅ Registry cleared.");
}