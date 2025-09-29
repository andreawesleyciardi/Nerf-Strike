#include <Color.h>
#include <RGBLed.h>
#include <Button.h>
#include <Battery.h>
#include <OPCodes.h>

#include "TargetConfig.h"
#include "WirelessManager.h"
#include "PairingManager.h"
#include "Sensor.h"
#include "RGBRing.h"
#include "SevenSegmentDisplay.h"
#include "Buzzer.h"

// Components
WirelessManager wireless;
PairingManager pairingManager(wireless);
// Leds
RGBLed statusRgbLed(statusRedLedPin, statusGreenLedPin, statusBlueLedPin);
RGBLed batteryRgbLed(batteryRedLedPin, batteryGreenLedPin, batteryBlueLedPin);
RGBRing rgbRing(ledRingPin, 24);
// Sensor
Sensor sensor(sensorComponentPin);
// Hardware
SevenSegmentDisplay scoreDisplay(displayDataPin, displayClockPin, displayLatchPin);
// Buttons
Button pairingResetButton(pairingResetButtonPin);
Button batteryButton(batteryButtonPin);
// Buzzer
Buzzer buzzer(buzzerPin);
// Battery
Battery battery(true);

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println(F("🔧 Target booting..."));

  randomSeed(analogRead(0));  // ✅ Better token randomness

  battery.setup();
  pairingResetButton.setup();
  batteryButton.setup();
  sensor.setup();
  statusRgbLed.setup();
  batteryRgbLed.setup();
  rgbRing.setup();
  buzzer.setup();
  scoreDisplay.setup(3);
  wireless.setup();

  Serial.println(F("✅ Radio initialized and chip connected."));
  Serial.println(F("Radio listening on pairing pipe."));

  pairingManager.pair();  // Stateless pairing

  // 🧠 Show pairing status on display
  uint8_t id = pairingManager.getAssignedID();
  if (id != 0xFF) {
    scoreDisplay.showScore(id);
    delay(1000);
    scoreDisplay.clear();
  }

  Serial.println(F("Setup complete."));
}

void loop() {
  static unsigned long lastHitTime = 0;
  static unsigned long lastHeartbeat = millis();
  static bool awaitingAck = false;
  static bool heartbeatLost = false;

  // 🔋 Battery
  if (batteryButton.wasPressed()) {
    battery.check(batteryVerificationFeedback);      // Manual check via button
  }
  battery.autoCheck(batteryVerificationFeedback);  // Automatic check every 5 minutes or 2.5 minutes if battery level is low

  if (pairingResetButton.wasPressed()) {
    Serial.println(F("🔍 Verifying connection with hub..."));
    bool verified = pairingManager.verify();
    if (verified) {
      Serial.println(F("✅ Connection verified."));
      statusRgbLed.blink("Green");
    } else {
      Serial.println(F("❌ Verification failed. Re-pairing..."));
      statusRgbLed.blink("Red");
      pairingManager.pair();
    }
    uint8_t id = pairingManager.getAssignedID();
    if (id != 0xFF) {
      scoreDisplay.showScore(id);
      delay(2000);
      scoreDisplay.clear();
    }
  }
  // 🛠 Manual token reset if button held > 3s
  if (pairingResetButton.wasLongPressed(feedback)) {
    Serial.println(F("🧹 Token reset triggered..."));
    pairingManager.resetToken();
    statusRgbLed.blink("Green", 3);
    delay(500);
    pairingManager.pair();
    awaitingAck = false;
    lastHeartbeat = millis();
    heartbeatLost = false;

    uint8_t id = pairingManager.getAssignedID();
    if (id != 0xFF) {
      scoreDisplay.showScore(id);
      delay(1000);
      scoreDisplay.clear();
    }
  }

  // 📥 Handle incoming packets
  if (wireless.available()) {
    // Serial.println(F("📥 Target received a packet."));

    byte packet[32];
    wireless.read(packet, sizeof(packet));

    if (packet[0] == OPCODE_HEARTBEAT) { // 💓 Heartbeat
      lastHeartbeat = millis();
      heartbeatLost = false;
      Serial.println(F("💓 Heartbeat received from hub."));
    }

    if (packet[0] == OPCODE_BLINK_COMMAND) { // 🔦 Blink command
      Serial.println(F("🔦 Blink command received."));
      uint8_t assignedID = pairingManager.getAssignedID();
      scoreDisplay.showScore(assignedID);
      statusRgbLed.blink("Blue");
      rgbRing.chase("Blue", 30);
      delay(1000);
      scoreDisplay.clear();
    }

    if (packet[0] == 0x06) { // 🎯 Score update
      uint8_t newScore = packet[1];
      Serial.print(F("🎯 New score received: "));
      Serial.println(newScore);
      scoreDisplay.updateScore(newScore);
    }
  }

  // 🎯 Hit detection
  uint8_t targetId = pairingManager.getAssignedID();
  if (targetId != 0xFF) {
    if (sensor.isHit()) {
      Serial.println(F("✅ Target got hit."));
      rgbRing.blink("Green");
      buzzer.beep(10);  // Simple 200ms beep

      if (wireless.sendHitPacket(targetId)) {
        Serial.println(F("📡 Hit packet sent to hub."));
        lastHitTime = millis();
        awaitingAck = false;
      } else {
        Serial.println(F("⚠️ No response from hub. Will retry pairing."));
        awaitingAck = true;
        lastHitTime = millis();
      }
    }

    if (awaitingAck && millis() - lastHitTime > 5000) {
      Serial.println(F("🔄 Signal lost after hit. Re-pairing..."));
      pairingManager.pair();
      awaitingAck = false;
      lastHeartbeat = millis();
      heartbeatLost = false;

      uint8_t id = pairingManager.getAssignedID();
      if (id != 0xFF) {
        scoreDisplay.showScore(id);
        delay(1000);
        scoreDisplay.clear();
      }
    }

    if (millis() - lastHeartbeat > 10000 && !heartbeatLost) {
      Serial.println(F("💔 No heartbeat. Re-pairing..."));
      heartbeatLost = true;
      statusRgbLed.blink("Red");
      pairingManager.pair();
      lastHeartbeat = millis();

      uint8_t id = pairingManager.getAssignedID();
      if (id != 0xFF) {
        scoreDisplay.showScore(id);
        delay(1000);
        scoreDisplay.clear();
      }
    }
  }
}

void feedback() {
  statusRgbLed.blink("Red");
}

void batteryVerificationFeedback(long batteryValue, bool wasAutomaticCheck) {
  if (batteryValue < 30) {
    if (batteryValue < 10) {
      batteryRgbLed.blink("Red", 100, 6);
    }
    else {
      batteryRgbLed.blink("Red");
    }
  }
  else if (batteryValue >= 30 && batteryValue < 60) {
    if (!wasAutomaticCheck) {
      batteryRgbLed.blink("Orange");
    }
  }
  else {
    if (!wasAutomaticCheck) {
      batteryRgbLed.blink("Green");
    }
  }
}