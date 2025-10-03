#ifndef WIRELESS_HUB_H
#define WIRELESS_HUB_H

#include <Arduino.h>
#include <RF24.h>
#include "HubConfig.h"
// #include <Protocol.h>
// #include <TargetType.h>
// #include "PairingRegistry.h"

class WirelessHub {
public:
  WirelessHub();
  void initialize();
  bool available();
  void read(byte* buffer, uint8_t length);
  bool isConnected();
  RF24& getRadio();

private:
  RF24 radio;
};

#endif



  // void sendPairingResponse(uint8_t assignedID, TargetType type);  // ✅ Updated
  // void sendVerificationResponse(uint8_t id);
  // void sendToTargetPipe(uint8_t id, const uint8_t* pipe, const byte* data, uint8_t length);

  // void triggerBlinkOnTargets(PairingRegistry& registry);
  // void sendHeartbeatToTargets(PairingRegistry& registry);