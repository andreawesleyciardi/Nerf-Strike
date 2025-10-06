#include "Receive.h"
#include <Protocol.h>
#include <OPCodes.h>

Receive::Receive(PairingRegistry& registry)
  : registry(registry) {}

const uint8_t Receive::pairingResponse(const byte* buffer) {
  
}

const uint8_t* Receive::verificationResponse(const byte* buffer) {
  
}

const uint8_t* Receive::scoreUpdate(const byte* buffer) {
  
}