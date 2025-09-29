#ifndef WIRELESS_MANAGER_H
#define WIRELESS_MANAGER_H

#include <Arduino.h>
#include <RF24.h>
#include <OPCodes.h>

class WirelessManager {
public:
  WirelessManager();
  void setup();
  void sendPairingRequest(uint32_t token);
  bool receivePairingResponse(uint8_t &assignedID);
  void sendVerificationRequest(uint8_t id);
  bool waitForVerificationAck(uint8_t id);
  void switchToTargetPipe(uint8_t id);
  void sendToHub(const byte* data, uint8_t length);
  bool available();
  void read(byte* buffer, uint8_t length);
  bool sendHitPacket(uint8_t targetId);

private:
  RF24 radio;
  const uint64_t pairingPipe = 0xF0F0F0F0E1LL;
  uint8_t targetPipe[6];
};

#endif
