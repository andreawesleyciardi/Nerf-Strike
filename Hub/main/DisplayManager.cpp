#include "DisplayManager.h"

DisplayManager::DisplayManager(uint8_t addr, uint8_t cols, uint8_t rows)
  : lcd(addr, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE), cols(cols), rows(rows) {}

void DisplayManager::setup() {
  lcd.begin(cols, rows);
  lcd.backlight();
  lcd.clear();
}

uint8_t DisplayManager::getRowCount() const {
  return rows;
}

void DisplayManager::showText(const String& text, bool scroll, uint16_t scrollDelay) {
  lcd.clear();
  if (!scroll || text.length() <= cols) {
    lcd.setCursor(0, 0);
    lcd.print(text);
    return;
  }

  for (int i = 0; i <= text.length() - cols; i++) {
    lcd.setCursor(0, 0);
    lcd.print(text.substring(i, i + cols));
    delay(scrollDelay);
  }
}

void DisplayManager::showMenuItem(const String& label, uint8_t index, uint8_t total) {
  lcd.clear();
  String line = String(index + 1) + "/" + String(total) + " " + label;
  printAligned(line, 0);
}

void DisplayManager::showButtonLabels(const String& leftLabel, const String& rightLabel) {
  lcd.setCursor(0, rows - 1);
  lcd.print(leftLabel);
  lcd.setCursor(cols - rightLabel.length(), rows - 1);
  lcd.print(rightLabel);
}

void DisplayManager::printAligned(const String& text, uint8_t row, const String& align) {
  int pad = 0;
  if (align == "center") {
    pad = (cols - text.length()) / 2;
  } else if (align == "right") {
    pad = cols - text.length();
  }
  lcd.setCursor(max(0, pad), row);
  lcd.print(text);
}

void DisplayManager::scrollIn(const String& text, uint8_t row, uint16_t speed) {
  lcd.clear();
  for (int i = 0; i <= text.length(); i++) {
    lcd.setCursor(0, row);
    lcd.print(text.substring(0, i));
    delay(speed);
  }
}

void DisplayManager::scrollOut(uint8_t row, uint16_t speed) {
  for (int i = cols; i >= 0; i--) {
    lcd.setCursor(0, row);
    lcd.print(String(' ', i));
    delay(speed);
  }
  clearRow(row);
}

void DisplayManager::fadeOut(uint8_t delayMs) {
  for (int i = 0; i < 3; i++) {
    lcd.noBacklight();
    delay(delayMs);
    lcd.backlight();
    delay(delayMs);
  }
  lcd.clear();
}

void DisplayManager::fadeIn(const String& text, uint8_t row, uint8_t delayMs) {
  lcd.clear();
  lcd.noBacklight();
  delay(delayMs);

  for (int i = 0; i < 3; i++) {
    lcd.backlight();
    delay(delayMs);
    lcd.noBacklight();
    delay(delayMs);
  }

  lcd.backlight();
  printAligned(text, row);
}

void DisplayManager::clearRow(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print(String(' ', cols));
}