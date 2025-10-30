#include "controller.h"
#include <algorithm>

void Controller::SetButtons(const std::array<bool, 8>& buttons) {
  this->buttons = buttons;
}

uint8_t Controller::Read() {
  uint8_t value = 0;
  if (index < 8 && buttons[index]) {
    value = 1;
  }
  index++;
  if ((strobe & 1) == 1) {
    index = 0;
  }
  return value;
}

void Controller::Write(uint8_t value) {
  strobe = value;
  if ((strobe & 1) == 1) {
    index = 0;
  }
}
