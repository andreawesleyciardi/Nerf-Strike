#include "CommandConsole.h"
#include "HubConfig.h"
#include <Arduino.h>

CommandConsole::CommandConsole(PairingRegistry& registry, WirelessHub& wireless)
  : registry(registry), wireless(wireless) {}

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
