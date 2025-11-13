#include "LcdDisplay.h"

LcdDisplay::LcdDisplay(uint8_t addr, uint8_t cols, uint8_t rows)
  : LiquidCrystal_I2C(addr, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE), cols(cols), rows(rows) {}

byte customArrowLeft[] = {
  B00110,
  B01000,
  B10000,
  B10000,
  B10000,
  B01010,
  B00110,
  B01110
};

byte customArrowRight[] = {
  B01100,
  B00010,
  B00001,
  B00001,
  B00001,
  B01010,
  B01100,
  B01110
};

// awg logo version 1
// byte name0x6[8] = { B00000, B00000, B00000, B00011, B00101, B01001, B01001, B00110 };
// byte name0x7[8] = { B00000, B00000, B00000, B01000, B01010, B01010, B01111, B11010 };
// byte name0x8[8] = { B00000, B00000, B00000, B10011, B11100, B10100, B00010, B00111 };
// byte name0x9[8] = { B00000, B00000, B00000, B00000, B10100, B11000, B10000, B00000 };
// byte name1x8[8] = { B01001, B10010, B11100, B00000, B00000, B00000, B00000, B00000 };

void LcdDisplay::setup() {
  begin(cols, rows);
  backlight();
  createChar(7, customArrowLeft);
  createChar(8, customArrowRight);

  // awg logo
  // createChar(1, name0x6);
  // createChar(2, name0x7);
  // createChar(3, name0x8);
  // createChar(4, name0x9);
  // createChar(5, name1x8);
    // display.setCursor(6, 1);
    // display.write(1);
    // display.setCursor(7, 1);
    // display.write(2);
    // display.setCursor(8, 1);
    // display.write(3);
    // display.setCursor(9, 1);
    // display.write(4);
    // display.setCursor(8, 2);
    // display.write(5);

  clear();
}

uint8_t LcdDisplay::getRowCount() const {
  return rows;
}

uint8_t LcdDisplay::getColumnCount() const {
  return cols;
}

void LcdDisplay::clear() {
  LiquidCrystal_I2C::clear();
}

void LcdDisplay::clearLine(uint8_t row) {
  setCursor(0, row);
  print(String(' ', cols));
}

void LcdDisplay::showLine(uint8_t row, const String& text, const String& align) {
  if (align == "center" || align == "right") {
    printAligned(text, row, align);
    return;
  }
  setCursor(0, row);
  print(text.substring(0, cols));
}

void LcdDisplay::showText(const String& text, bool scroll, uint16_t scrollDelay) {
  clear();
  if (!scroll || text.length() <= cols) {
    setCursor(0, 0);
    print(text);
    return;
  }

  for (int i = 0; i <= text.length() - cols; i++) {
    setCursor(0, 0);
    print(text.substring(i, i + cols));
    delay(scrollDelay);
  }
}

void LcdDisplay::showMenuItem(const String& label, uint8_t index, uint8_t total) {
  clear();
  String line = String(index + 1) + "/" + String(total) + " " + label;
  printAligned(line, 0);
}

void LcdDisplay::showButtonLabels(const ButtonLabels& labels) {
  String leftLabel = String(labels.left).length() > 0 ? String(labels.left) : "";
  String encoderLabel = String(labels.encoder).length() > 0 ? String(labels.encoder) : "";
  String rightLabel = String(labels.right).length() > 0 ? String(labels.right) : "";

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

  if (encoderLabel != "") {
    setCursor(9, 3);
    write(1);
    write(2);
  }
}

void LcdDisplay::printAligned(const String& text, uint8_t row, const String& align) {
  int pad = 0;
  if (align == "center") {
    pad = (cols - text.length()) / 2;
  } else if (align == "right") {
    pad = cols - text.length();
  }
  setCursor(max(0, pad), row);
  print(text);
}

void LcdDisplay::printAt(const String& text, uint8_t row, uint8_t col) {
  setCursor(col, row);
  print(text);
}

void LcdDisplay::scrollIn(const String& text, uint8_t row, uint16_t speed) {
  clear();
  for (int i = 0; i <= text.length(); i++) {
    setCursor(0, row);
    print(text.substring(0, i));
    delay(speed);
  }
}

void LcdDisplay::scrollOut(uint8_t row, uint16_t speed) {
  for (int i = cols; i >= 0; i--) {
    setCursor(0, row);
    print(String(' ', i));
    delay(speed);
  }
  clearLine(row);
}

void LcdDisplay::fadeOut(uint8_t delayMs) {
  for (int i = 0; i < 3; i++) {
    noBacklight();
    delay(delayMs);
    backlight();
    delay(delayMs);
  }
  clear();
}

void LcdDisplay::fadeIn(const String& text, uint8_t row, uint8_t delayMs) {
  clear();
  noBacklight();
  delay(delayMs);

  for (int i = 0; i < 3; i++) {
    backlight();
    delay(delayMs);
    noBacklight();
    delay(delayMs);
  }

  backlight();
  printAligned(text, row);
}




// #include "LcdDisplay.h"

// LcdDisplay::LcdDisplay(uint8_t addr, uint8_t cols, uint8_t rows)
//   : lcd(addr, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE), cols(cols), rows(rows) {}

// byte customArrowLeft[] = {
//   B00011,
//   B00100,
//   B01000,
//   B01000,
//   B01000,
//   B00101,
//   B00011,
//   B00111
// };

// byte customArrowRight[] = {
//   B11000,
//   B00100,
//   B00010,
//   B00010,
//   B00010,
//   B10100,
//   B11000,
//   B11100
// };

// void LcdDisplay::setup() {
//   lcd.begin(cols, rows);
//   lcd.backlight();
//   lcd.createChar(0, customArrowLeft);
//   lcd.createChar(1, customArrowRight);
//   lcd.clear();
// }

// uint8_t LcdDisplay::getRowCount() const {
//   return rows;
// }

// uint8_t LcdDisplay::getColumnCount() const {
//   return cols;
// }

// void LcdDisplay::clear() {
//   lcd.clear();
// }

// void LcdDisplay::clearLine(uint8_t row) {
//   lcd.setCursor(0, row);
//   lcd.print(String(' ', cols));
// }

// void LcdDisplay::showLine(uint8_t row, const String& text, const String& align) {
//   if (align == "center" || align == "right") {
//     printAligned(text, row, align);
//     return;
//   }
//   lcd.setCursor(0, row);
//   lcd.print(text.substring(0, cols));
// }

// void LcdDisplay::showText(const String& text, bool scroll, uint16_t scrollDelay) {
//   lcd.clear();
//   if (!scroll || text.length() <= cols) {
//     lcd.setCursor(0, 0);
//     lcd.print(text);
//     return;
//   }

//   for (int i = 0; i <= text.length() - cols; i++) {
//     lcd.setCursor(0, 0);
//     lcd.print(text.substring(i, i + cols));
//     delay(scrollDelay);
//   }
// }

// void LcdDisplay::showMenuItem(const String& label, uint8_t index, uint8_t total) {
//   lcd.clear();
//   String line = String(index + 1) + "/" + String(total) + " " + label;
//   printAligned(line, 0);
// }

// void LcdDisplay::showButtonLabels(const ButtonLabels& labels) {
//   // String leftLabel = String(labels.left).length() > 0 ? "[" + String(labels.left) + "]" : "";
//   // String encoderLabel = String(labels.encoder).length() > 0 ? "[" + String(labels.encoder) + "]" : "";
//   // String rightLabel = String(labels.right).length() > 0 ? "[" + String(labels.right) + "]" : "";
//   String leftLabel = String(labels.left).length() > 0 ? String(labels.left) : "";
//   String encoderLabel = String(labels.encoder).length() > 0 ? String(labels.encoder) : "";
//   String rightLabel = String(labels.right).length() > 0 ? String(labels.right) : "";

//   if (strcmp(labels.alignment, "center") == 0) {
//     String line = leftLabel + " " + encoderLabel + " " + rightLabel;
//     printAligned(line, getRowCount() - 1, "center");
//     return;
//   }

//   if (strcmp(labels.alignment, "left") == 0) {
//     String line = leftLabel + " " + encoderLabel + " " + rightLabel;
//     showLine(getRowCount() - 1, line);
//     return;
//   }

//   // Default: spread
//   int totalWidth = getColumnCount();
//   int leftPos = 0;
//   int rightPos = totalWidth - rightLabel.length();
//   int encoderPos = (totalWidth - encoderLabel.length()) / 2;

//   clearLine(getRowCount() - 1);
//   if (leftLabel.length() > 0) printAt(leftLabel, getRowCount() - 1, leftPos);
//   if (encoderLabel.length() > 0) printAt(encoderLabel, getRowCount() - 1, encoderPos);
//   if (rightLabel.length() > 0) printAt(rightLabel, getRowCount() - 1, rightPos);
// }

// void LcdDisplay::printAligned(const String& text, uint8_t row, const String& align) {
//   int pad = 0;
//   if (align == "center") {
//     pad = (cols - text.length()) / 2;
//   } else if (align == "right") {
//     pad = cols - text.length();
//   }
//   lcd.setCursor(max(0, pad), row);
//   lcd.print(text);
// }

// void LcdDisplay::printAt(const String& text, uint8_t row, uint8_t col) {
//   lcd.setCursor(col, row);
//   lcd.print(text);
// }

// void LcdDisplay::scrollIn(const String& text, uint8_t row, uint16_t speed) {
//   lcd.clear();
//   for (int i = 0; i <= text.length(); i++) {
//     lcd.setCursor(0, row);
//     lcd.print(text.substring(0, i));
//     delay(speed);
//   }
// }

// void LcdDisplay::scrollOut(uint8_t row, uint16_t speed) {
//   for (int i = cols; i >= 0; i--) {
//     lcd.setCursor(0, row);
//     lcd.print(String(' ', i));
//     delay(speed);
//   }
//   clearLine(row);
// }

// void LcdDisplay::fadeOut(uint8_t delayMs) {
//   for (int i = 0; i < 3; i++) {
//     lcd.noBacklight();
//     delay(delayMs);
//     lcd.backlight();
//     delay(delayMs);
//   }
//   lcd.clear();
// }

// void LcdDisplay::fadeIn(const String& text, uint8_t row, uint8_t delayMs) {
//   lcd.clear();
//   lcd.noBacklight();
//   delay(delayMs);

//   for (int i = 0; i < 3; i++) {
//     lcd.backlight();
//     delay(delayMs);
//     lcd.noBacklight();
//     delay(delayMs);
//   }

//   lcd.backlight();
//   printAligned(text, row);
// }