#ifndef WIRELESS_HUB_H
#define WIRELESS_HUB_H

#include <Arduino.h>
#include <RF24.h>
#include "HubConfig.h"

class WirelessHub {
public:
  WirelessHub();
  void initialize();
  bool available();
  void read(byte* buffer, uint8_t length);
  void sendPairingResponse(uint8_t assignedID);
  void sendVerificationAck(uint8_t id);
  void sendToTargetPipe(uint8_t id, const uint8_t* pipe, const byte* data, uint8_t length);
  bool isConnected();

private:
  RF24 radio;
};

#endif
