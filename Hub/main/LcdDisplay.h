#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

class LcdDisplay {
public:
  LcdDisplay(uint8_t addr = 0x3F, uint8_t cols = 20, uint8_t rows = 4);
  void setup();
  uint8_t getRowCount() const;

  // Text rendering
  void clear();
  void clearRow(uint8_t row);
  void showLine(uint8_t row, const String& text);
  void showText(const String& text, bool scroll = false, uint16_t scrollDelay = 300);
  void showMenuItem(const String& label, uint8_t index, uint8_t total);
  void showButtonLabels(const String& leftLabel, const String& rightLabel);

  // Alignment
  void printAligned(const String& text, uint8_t row = 0, const String& align = "left");

  // Transitions
  void scrollIn(const String& text, uint8_t row = 0, uint16_t speed = 50);
  void scrollOut(uint8_t row = 0, uint16_t speed = 50);
  void fadeOut(uint8_t delayMs = 30);
  void fadeIn(const String& text, uint8_t row = 0, uint8_t delayMs = 30);

private:
  LiquidCrystal_I2C lcd;
  uint8_t cols;
  uint8_t rows;
};

#endif