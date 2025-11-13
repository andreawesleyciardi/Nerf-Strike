#include <Protocol.h>
#include <OPCodes.h>

#include "CommandConsole.h"
#include "TargetTypeManager.h"

CommandConsole::CommandConsole(PairingRegistry& registry, Send& send, RotaryEncoder& encoder, Button& left, Button& right, TargetTypeManager& typeManager, Communication& communication)
  : registry(registry), send(send), encoder(encoder), leftButton(left), rightButton(right), targetTypeManager(typeManager), communication(communication) {}

void CommandConsole::processSerial() {
  if (!Serial.available()) return;

  String command = Serial.readStringUntil('\n');
  command.trim();

  if (command == "help") {
    Serial.println(F("🧠 Available commands:"));
    Serial.println(F("  blink           → Trigger blink on all targets"));
    Serial.println(F("  settype <tier>  → Set allowed target type (StrikeLite, StrikeVision, StrikeAI)"));
    Serial.println(F("  gettype         → Show current allowed target type"));
    return;
  }

  if (command == "blink") {
    communication.blinkAll();
    return;
  }

  if (command.startsWith("settype ")) {
    String arg = command.substring(8);
    arg.trim();

    if (arg == "StrikeLite") {
      targetTypeManager.setAllowedType(TargetType::StrikeLite);
    } else if (arg == "StrikeVision") {
      targetTypeManager.setAllowedType(TargetType::StrikeVision);
    } else if (arg == "StrikeAI") {
      targetTypeManager.setAllowedType(TargetType::StrikeAI);
    } else {
      Serial.println(F("❌ Unknown target type. Use StrikeLite, StrikeVision, or StrikeAI."));
      return;
    }

    targetTypeManager.saveToEEPROM();
    Serial.print(F("✅ Allowed target type set to: "));
    Serial.println(targetTypeToString(targetTypeManager.getAllowedType()));
    return;
  }

  if (command == "gettype") {
    Serial.print(F("📦 Current allowed target type: "));
    Serial.println(targetTypeToString(targetTypeManager.getAllowedType()));
    return;
  }

  Serial.println(F("❌ Unknown command. Type 'help' for a list of commands."));
}