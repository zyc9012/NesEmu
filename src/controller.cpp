#include "controller.h"
#include "string.h"

Controller::Controller()
{
}


Controller::~Controller()
{
}

void Controller::SetButtons(bool buttons[8]) {
  memcpy(this->buttons, buttons, sizeof(bool)*8);
}

uint8_t Controller::Read() {
  uint8_t value = 0;
  if (index < 8 && buttons[index]) {
    value = 1;
  }
  index++;
  if ((strobe&1) == 1) {
    index = 0;
  }
  return value;
}

void Controller::Write(uint8_t value) {
  strobe = value;
  if ((strobe&1) == 1) {
    index = 0;
  }
}
