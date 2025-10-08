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
  bool isConnected();
  RF24& getRadio();

private:
  RF24 radio;
};

#endif