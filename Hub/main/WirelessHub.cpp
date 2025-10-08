#include "WirelessHub.h"

WirelessHub::WirelessHub() : radio(transmitterCEPin, transmitterCSNPin) {}

void WirelessHub::initialize() {
  Serial.println(F("🔧 Initializing hub radio..."));

  if (!radio.begin()) {
    Serial.println(F("❌ Radio failed to initialize."));
    return;
  }

  if (!radio.isChipConnected()) {
    Serial.println(F("❌ Radio chip not connected."));
    return;
  }

  Serial.println(F("✅ Hub radio initialized."));

  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.setRetries(5, 15);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(100);

  radio.openReadingPipe(0, pairingPipe);
  radio.startListening();

  Serial.println(F("📡 Hub listening on pairing pipe."));
}

RF24& WirelessHub::getRadio() {
  return radio;
}

bool WirelessHub::available() {
  return radio.available();
}

void WirelessHub::read(byte* buffer, uint8_t length) {
  radio.read(buffer, length);
}



bool WirelessHub::isConnected() {
  return radio.isChipConnected();
}