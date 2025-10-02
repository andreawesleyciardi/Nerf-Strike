#ifndef COMMAND_CONSOLE_H
#define COMMAND_CONSOLE_H

#include <Arduino.h>
#include "PairingRegistry.h"
#include "WirelessHub.h"
#include <RotaryEncoder.h>
#include <Button.h>
#include "TargetTypeManager.h"

class CommandConsole {
public:
  CommandConsole(PairingRegistry& registry, WirelessHub& wireless, RotaryEncoder& encoder, Button& leftButton, Button& rightButton, TargetTypeManager& typeManager);
  void processSerial();
  void processInput();  // 🧠 Unified input handler

private:
  PairingRegistry& registry;
  WirelessHub& wireless;
  RotaryEncoder& encoder;
  Button& leftButton;
  Button& rightButton;
  TargetTypeManager& targetTypeManager;

  bool verbose = true;
  unsigned long startTime;

  int menuIndex = 0;
  const int maxMenuItems = 3;

  // ✅ Active commands
  void dumpRegistry();             // 'list'
  void clearRegistry();           // 'clear'
  void testTargets();             // 'test'

  // ✅ Messaging and diagnostics
  void sendMessage(String idStr, String msgStr);
  void pingTarget(String idStr);
  void broadcastMessage(String msgStr);
  void showTargetInfo(String idStr);

  // ✅ System status and control
  void showStatus();
  void toggleLogging(String arg);
  void rebootHub();
  void factoryReset();
  void showVersion();
  void showUptime();
  void showHelp();

  // ✅ Menu interaction
  void showMenu();
  void selectMenuItem(int index);
  void cancelMenu();
  void confirmMenu();
};

#endif