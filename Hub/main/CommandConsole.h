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

  void dumpRegistry();
  void clearRegistry();
  void sendMessage(String idStr, String msgStr);
  void pingTarget(String idStr);
  void broadcastMessage(String msgStr);
  void showStatus();
  void toggleLogging(String arg);
  void rebootHub();
  void factoryReset();
  void showVersion();
  void showUptime();
  void showTargetInfo(String idStr);
  void simulateEvent(String eventStr);
  void showHelp();
};

#endif
