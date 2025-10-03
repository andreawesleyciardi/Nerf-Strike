#include "WirelessHub.h"
#include <OPCodes.h>
#include <Protocol.h>

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

void WirelessHub::sendPairingResponse(uint8_t assignedID, TargetType type) {
  PairingResponse response = {
    OPCODE_PAIRING_RESPONSE,
    assignedID,
    type
  };

  Serial.println(F("📡 Hub sending pairing response on pairing pipe..."));

  radio.stopListening();
  radio.openWritingPipe(pairingPipe);
  bool success = radio.write(&response, sizeof(response));
  delay(100);  // ✅ Give target time to switch pipes
  radio.startListening();

  if (success) {
    Serial.print(F("✅ Sent pairing response with ID: "));
    Serial.print(assignedID);
    Serial.print(F(" | Type: "));
    Serial.println(targetTypeToString(type));
  } else {
    Serial.println(F("❌ Failed to send pairing response."));
  }
}

void WirelessHub::sendVerificationResponse(uint8_t id) {
  VerificationResponse response = {
    OPCODE_VERIFICATION_RESPONSE,
    id
  };

  radio.stopListening();
  radio.openWritingPipe(pairingPipe);
  bool success = radio.write(&response, sizeof(response));
  radio.startListening();

  if (success) {
    Serial.print(F("✅ Sent verification response for ID: "));
    Serial.println(id);
  } else {
    Serial.print(F("❌ Failed to send verification response for ID: "));
    Serial.println(id);
  }
}

// Utilities functions ---------------------------------------------------------------------------------

bool WirelessHub::available() {
  return radio.available();
}

void WirelessHub::read(byte* buffer, uint8_t length) {
  radio.read(buffer, length);
}

void WirelessHub::sendToTargetPipe(uint8_t id, const uint8_t* pipe, const byte* data, uint8_t length) {
  radio.stopListening();
  radio.openWritingPipe(pipe);
  delay(5);  // ✅ Give target time to listen

  bool success = radio.write(data, length);
  radio.startListening();

  if (success) {
    Serial.print("📤 Sent data to target ID ");
    Serial.print(id);
    Serial.print(" via pipe ");
    Serial.println((char*)pipe);
  } else {
    Serial.println(F("❌ Failed to send data to target."));
  }
}

bool WirelessHub::isConnected() {
  return radio.isChipConnected();
}