#include "Communication.h"
#include <Protocol.h>
#include <OPCodes.h>
#include <DisplayFeedback.h>

Communication::Communication(Receive& receive, Send& send, PairingRegistry& registry, RGBLed& statusRgbLed)
  : receive(receive), send(send), registry(registry), statusRgbLed(statusRgbLed) {}

void Communication::pairing(const byte* buffer) {
  
}

void Communication::verification(const byte* buffer) {
  c
}

void Communication::hit(const byte* buffer) {
  
}