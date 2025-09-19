#include "CommandConsole.h"
#include <avr/wdt.h>

CommandConsole::CommandConsole(PairingRegistry& reg, WirelessHub& wh)
  : registry(reg), wireless(wh), startTime(millis()) {}

void CommandConsole::processSerial() {
  if (!Serial.available()) return;

  String command = Serial.readStringUntil('\n');
  command.trim();

  if (command.equalsIgnoreCase("dump")) {
    dumpRegistry();
  } else if (command.equalsIgnoreCase("clear")) {
    clearRegistry();
  } else if (command.startsWith("send ")) {
    int space1 = command.indexOf(' ');
    int space2 = command.indexOf(' ', space1 + 1);
    if (space2 > space1) {
      sendMessage(command.substring(space1 + 1, space2), command.substring(space2 + 1));
    } else {
      Serial.println("⚠️ Usage: send <id> <message>");
    }
  } else if (command.startsWith("ping ")) {
    pingTarget(command.substring(5));
  } else if (command.startsWith("broadcast ")) {
    broadcastMessage(command.substring(10));
  } else if (command.equalsIgnoreCase("status")) {
    showStatus();
  } else if (command.startsWith("log ")) {
    toggleLogging(command.substring(4));
  } else if (command.equalsIgnoreCase("reboot")) {
    rebootHub();
  } else if (command.equalsIgnoreCase("factory")) {
    factoryReset();
  } else if (command.equalsIgnoreCase("version")) {
    showVersion();
  } else if (command.equalsIgnoreCase("uptime")) {
    showUptime();
  } else if (command.startsWith("targetinfo ")) {
    showTargetInfo(command.substring(11));
  } else if (command.startsWith("simulate ")) {
    simulateEvent(command.substring(9));
  } else if (command.equalsIgnoreCase("help")) {
    showHelp();
  } else {
    Serial.println("⚠️ Unknown command. Type 'help' for a list.");
  }
}

void CommandConsole::dumpRegistry() {
  Serial.println("📦 Dumping pairing registry...");
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint32_t token = registry.getTokenAt(i);
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);

    if (id != 0xFF) {
      Serial.print("🔹 ID: ");
      Serial.print(id);
      Serial.print(" | Token: ");
      Serial.print(token);
      Serial.print(" | Pipe: ");
      Serial.println((char*)pipe);
    }
  }
}

void CommandConsole::clearRegistry() {
  registry.clearRegistry();
}

void CommandConsole::sendMessage(String idStr, String msgStr) {
  uint8_t id = idStr.toInt();
  const uint8_t* pipe = registry.getPipeForID(id);
  if (!pipe) {
    Serial.println("❌ Unknown ID. Cannot send.");
    return;
  }

  byte msg[32];
  uint8_t len = msgStr.length();
  if (len > 31) len = 31;
  memcpy(msg, msgStr.c_str(), len);
  msg[len] = '\0';

  wireless.sendToTargetPipe(id, pipe, msg, len + 1);
}

void CommandConsole::pingTarget(String idStr) {
  uint8_t id = idStr.toInt();
  const uint8_t* pipe = registry.getPipeForID(id);
  if (!pipe) {
    Serial.println("❌ Unknown ID. Cannot ping.");
    return;
  }

  byte msg[2] = {0x04, id};
  wireless.sendToTargetPipe(id, pipe, msg, sizeof(msg));
}

void CommandConsole::broadcastMessage(String msgStr) {
  byte msg[32];
  uint8_t len = msgStr.length();
  if (len > 31) len = 31;
  memcpy(msg, msgStr.c_str(), len);
  msg[len] = '\0';

  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    uint8_t id = registry.getIDAt(i);
    const uint8_t* pipe = registry.getPipeForID(id);
    if (id != 0xFF && pipe) {
      wireless.sendToTargetPipe(id, pipe, msg, len + 1);
    }
  }
}

void CommandConsole::showStatus() {
  Serial.println("📊 Hub Status:");
  Serial.print("🔧 Radio connected: ");
  Serial.println(wireless.isConnected() ? "Yes" : "No");

  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    if (registry.getIDAt(i) != 0xFF) count++;
  }
  Serial.print("📋 Paired targets: ");
  Serial.println(count);
}

void CommandConsole::toggleLogging(String arg) {
  arg.toLowerCase();
  if (arg == "on") {
    verbose = true;
    Serial.println("📝 Verbose logging enabled.");
  } else if (arg == "off") {
    verbose = false;
    Serial.println("🔕 Verbose logging disabled.");
  } else {
    Serial.println("⚠️ Usage: log on/off");
  }
}

void CommandConsole::rebootHub() {
  Serial.println("🔁 Rebooting hub...");
  delay(100);
  wdt_enable(WDTO_15MS);
  while (true) {}
}

void CommandConsole::factoryReset() {
  Serial.println("🧯 Performing factory reset...");
  clearRegistry();
  rebootHub();
}

void CommandConsole::showVersion() {
  Serial.println("🆕 Firmware Version: 1.0.0");
}

void CommandConsole::showUptime() {
  unsigned long uptimeMs = millis() - startTime;
  unsigned long seconds = uptimeMs / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  Serial.print("⏱️ Uptime: ");
  Serial.print(hours);
  Serial.print("h ");
  Serial.print(minutes % 60);
  Serial.print("m ");
  Serial.print(seconds % 60);
  Serial.println("s");
}

void CommandConsole::showTargetInfo(String idStr) {
  uint8_t id = idStr.toInt();
  const uint8_t* pipe = registry.getPipeForID(id);
  uint32_t token = 0;

  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    if (registry.getIDAt(i) == id) {
      token = registry.getTokenAt(i);
      break;
    }
  }

  if (pipe && token != 0) {
    Serial.print("🔍 Target ID: ");
    Serial.println(id);
    Serial.print("🔑 Token: ");
    Serial.println(token);
    Serial.print("📡 Pipe: ");
    Serial.println((char*)pipe);
  } else {
    Serial.println("❌ Target not found.");
  }
}

void CommandConsole::simulateEvent(String eventStr) {
  eventStr.toLowerCase();
  if (eventStr == "hit") {
    Serial.println("🎯 Simulating target hit...");
    // Future: trigger game logic
  } else if (eventStr == "timeout") {
    Serial.println("⏳ Simulating timeout...");
    // Future: trigger timeout logic
  } else {
    Serial.println("⚠️ Unknown event. Try 'hit' or 'timeout'.");
  }
}

void CommandConsole::showHelp() {
  Serial.println("📘 Available Commands:");
  Serial.println("──────────────────────────────────────────────");
  Serial.println("dump                  → Show all paired targets");
  Serial.println("clear                 → Wipe registry from EEPROM");
  Serial.println("send <id> <msg>       → Send message to specific target");
  Serial.println("ping <id>             → Send ping to target");
  Serial.println("broadcast <msg>       → Send message to all targets");
  Serial.println("status                → Show hub status");
  Serial.println("log on/off            → Toggle verbose logging");
  Serial.println("reboot                → Soft reboot the hub");
  Serial.println("factory               → Clear registry and reboot");
  Serial.println("version               → Show firmware version");
  Serial.println("uptime                → Show hub uptime");
  Serial.println("targetinfo <id>       → Show details for a target");
  Serial.println("simulate <event>      → Simulate system event");
  Serial.println("help                  → Show this command list");
  Serial.println("──────────────────────────────────────────────");
}
