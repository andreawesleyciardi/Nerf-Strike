#include "WirelessTarget.h"

WirelessTarget::WirelessTarget() : radio(transmitterCEPin, transmitterCSNPin) {}

void WirelessTarget::setup() {
  Serial.println(F("🔧 Initializing radio..."));

  if (!radio.begin()) {
    Serial.println(F("❌ Radio failed to initialize. Check wiring and power."));
    return;
  }

  if (!radio.isChipConnected()) {
    Serial.println(F("❌ Radio chip not connected. Check CE/CSN and SPI pins."));
    return;
  }

  Serial.println(F("✅ Radio initialized and chip connected."));

  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.setRetries(5, 15);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(100);

  radio.openReadingPipe(0, pairingPipe);
  radio.startListening();

  Serial.println(F("📡 Radio listening on active pairing pipe."));
}

// void WirelessTarget::sendVerificationRequest(uint8_t id) {
//   VerificationRequestPacket request = {
//     OPCODE_VERIFICATION_REQUEST,
//     id
//   };

//   Serial.print(F("📨 Sending verification request for ID "));
//   Serial.println(id);

//   radio.stopListening();
//   radio.openWritingPipe(pairingPipe);
//   bool success = radio.write(&request, sizeof(request));
//   radio.startListening();

//   if (success) {
//     Serial.println(F("✅ Verification request sent."));
//   } else {
//     Serial.println(F("❌ Failed to send verification request."));
//   }
// }

// bool WirelessTarget::waitForVerificationResponse(uint8_t id) {
//   Serial.println(F("⏳ Waiting for verification acknowledgment..."));
//   unsigned long startTime = millis();

//   while (millis() - startTime < 1000) {
//     if (radio.available()) {
//       VerificationResponsePacket response;
//       radio.read(&response, sizeof(response));

//       if (response.opcode == OPCODE_VERIFICATION_RESPONSE && response.id == id) {
//         Serial.print(F("✅ Verification acknowledged by hub. ID: "));
//         Serial.println(response.id);
//         return true;
//       }
//     }
//   }

//   Serial.println(F("❌ No verification acknowledgment received."));
//   return false;
// }

// bool WirelessTarget::sendHitPacket(uint8_t id) {
//   HitEventPacket packet = {
//     OPCODE_HIT_EVENT,
//     id
//   };

//   radio.stopListening();
//   bool success = radio.write(&packet, sizeof(packet));
//   radio.startListening();

//   if (success) {
//     Serial.println(F("📡 Hit packet sent successfully."));
//     return true;
//   } else {
//     Serial.println(F("❌ Failed to send hit packet."));
//     return false;
//   }
// }

// Utilities functions ---------------------------------------------------------------------------------



// void WirelessTarget::sendToHub(const byte* data, uint8_t length) {
//   radio.stopListening();
//   radio.openWritingPipe(targetPipe);
//   bool success = radio.write(data, length);
//   radio.startListening();

//   if (success) {
//     Serial.println(F("📤 Sent data to hub via target pipe."));
//   } else {
//     Serial.println(F("❌ Failed to send data to hub."));
//   }
// }

bool WirelessTarget::available() {
  return radio.available();
}

void WirelessTarget::read(byte* buffer, uint8_t length) {
  radio.read(buffer, length);
}

RF24& WirelessTarget::getRadio() {
  return radio;
}