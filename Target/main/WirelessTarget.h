#ifndef WIRELESS_TARGET_H
#define WIRELESS_TARGET_H

#include <Arduino.h>
#include <RF24.h>
#include <Protocol.h>
#include <TargetType.h>
#include <OPCodes.h>

class WirelessTarget {
public:
  WirelessTarget();
  void setup();

  void sendPairingRequest(uint32_t token);
  bool receivePairingResponse(uint8_t &assignedID); // Legacy fallback
  bool receivePairingResponse(uint8_t &assignedID, TargetType &typeOut); // ✅ Updated version

  void sendVerificationRequest(uint8_t id);
  bool waitForVerificationResponse(uint8_t id);

  void switchToTargetPipe(uint8_t id);
  void sendToHub(const byte* data, uint8_t length);

  bool sendHitPacket(uint8_t targetId);

  bool available();
  void read(byte* buffer, uint8_t length);

  RF24& getRadio();

private:
  RF24 radio;
  uint8_t targetPipe[6];
};

#endif