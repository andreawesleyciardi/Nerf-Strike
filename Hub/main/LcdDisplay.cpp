#include "LcdDisplay.h"

LcdDisplay::LcdDisplay(uint8_t addr, uint8_t cols, uint8_t rows)
  : lcd(addr, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE), cols(cols), rows(rows) {}

void LcdDisplay::setup() {
  lcd.begin(cols, rows);
  lcd.backlight();
  lcd.clear();
}

uint8_t LcdDisplay::getRowCount() const {
  return rows;
}

uint8_t LcdDisplay::getColumnCount() const {
  return cols;
}

void LcdDisplay::clear() {
  lcd.clear();
}

void LcdDisplay::clearLine(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print(String(' ', cols));
}

void LcdDisplay::showLine(uint8_t row, const String& text, const String& align) {
  if (align == "center" || align == "right") {
    printAligned(text, row, align);
    return;
  }
  lcd.setCursor(0, row);
  lcd.print(text.substring(0, cols));
}

void LcdDisplay::showText(const String& text, bool scroll, uint16_t scrollDelay) {
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

void LcdDisplay::showMenuItem(const String& label, uint8_t index, uint8_t total) {
  lcd.clear();
  String line = String(index + 1) + "/" + String(total) + " " + label;
  printAligned(line, 0);
}

void LcdDisplay::showButtonLabels(const ButtonLabels& labels) {
  String leftLabel = String(labels.left).length() > 0 ? "[" + String(labels.left) + "]" : "";
  String encoderLabel = String(labels.encoder).length() > 0 ? "[" + String(labels.encoder) + "]" : "";
  String rightLabel = String(labels.right).length() > 0 ? "[" + String(labels.right) + "]" : "";

  if (strcmp(labels.alignment, "center") == 0) {
    String line = leftLabel + " " + encoderLabel + " " + rightLabel;
    printAligned(line, getRowCount() - 1, "center");
    return;
  }

  if (strcmp(labels.alignment, "left") == 0) {
    String line = leftLabel + " " + encoderLabel + " " + rightLabel;
    showLine(getRowCount() - 1, line);
    return;
  }

  // Default: spread
  int totalWidth = getColumnCount();
  int leftPos = 0;
  int rightPos = totalWidth - rightLabel.length();
  int encoderPos = (totalWidth - encoderLabel.length()) / 2;

  clearLine(getRowCount() - 1);
  if (leftLabel.length() > 0) printAt(leftLabel, getRowCount() - 1, leftPos);
  if (encoderLabel.length() > 0) printAt(encoderLabel, getRowCount() - 1, encoderPos);
  if (rightLabel.length() > 0) printAt(rightLabel, getRowCount() - 1, rightPos);
}

void LcdDisplay::printAligned(const String& text, uint8_t row, const String& align) {
  int pad = 0;
  if (align == "center") {
    pad = (cols - text.length()) / 2;
  } else if (align == "right") {
    pad = cols - text.length();
  }
  lcd.setCursor(max(0, pad), row);
  lcd.print(text);
}

void LcdDisplay::printAt(const String& text, uint8_t row, uint8_t col) {
  lcd.setCursor(col, row);
  lcd.print(text);
}

void LcdDisplay::scrollIn(const String& text, uint8_t row, uint16_t speed) {
  lcd.clear();
  for (int i = 0; i <= text.length(); i++) {
    lcd.setCursor(0, row);
    lcd.print(text.substring(0, i));
    delay(speed);
  }
}

void LcdDisplay::scrollOut(uint8_t row, uint16_t speed) {
  for (int i = cols; i >= 0; i--) {
    lcd.setCursor(0, row);
    lcd.print(String(' ', i));
    delay(speed);
  }
  clearLine(row);
}

void LcdDisplay::fadeOut(uint8_t delayMs) {
  for (int i = 0; i < 3; i++) {
    lcd.noBacklight();
    delay(delayMs);
    lcd.backlight();
    delay(delayMs);
  }
  lcd.clear();
}

void LcdDisplay::fadeIn(const String& text, uint8_t row, uint8_t delayMs) {
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