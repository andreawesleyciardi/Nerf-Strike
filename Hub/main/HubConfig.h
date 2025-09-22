#ifndef HUB_CONFIG_H
#define HUB_CONFIG_H

#include <stdint.h>

#define MAX_TARGETS 10

const uint8_t transmitterCEPin = 6;
const uint8_t transmitterCSNPin = 7;

const uint64_t pairingPipe = 0xF0F0F0F0E1LL;

struct PairingRequest {
  uint32_t token;
  bool hello;
};

struct PairingResponse {
  uint8_t assignedID;
};

#endif
