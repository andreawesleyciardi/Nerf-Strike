#ifndef COMMAND_CONSOLE_H
#define COMMAND_CONSOLE_H

#include <Arduino.h>
#include "PairingRegistry.h"
#include "WirelessHub.h"

class CommandConsole {
public:
  CommandConsole(PairingRegistry& registry, WirelessHub& wireless);
  void processSerial();

private:
  PairingRegistry& registry;
  WirelessHub& wireless;
  bool verbose = true;
  unsigned long startTime;

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

  // ❌ Legacy or unused logic (commented out)
  /*
  void simulateEvent(String eventStr);
  void resetEEPROM();
  void loadRegistryFromEEPROM();
  void saveRegistryToEEPROM();
  */
};

#endif
