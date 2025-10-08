#ifndef BUTTON_LABELS_H
#define BUTTON_LABELS_H

struct ButtonLabels {
  const char* left;
  const char* encoder;
  const char* right;
  const char* alignment = "spread";  // "spread", "center", "left"

  ButtonLabels(const char* l, const char* e, const char* r, const char* a = "spread")
    : left(l), encoder(e), right(r), alignment(a) {}
};

#endif