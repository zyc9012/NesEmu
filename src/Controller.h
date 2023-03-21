#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <stdint.h>

enum ControllerButtons {
  ButtonA,
  ButtonB,
  ButtonSelect,
  ButtonStart,
  ButtonUp,
  ButtonDown,
  ButtonLeft,
  ButtonRight,
};

class Controller
{
public:
  Controller();
  ~Controller();
  void SetButtons(bool buttons[8]);
  uint8_t Read();
  void Write(uint8_t value);

  bool buttons[8] = { false, };
  uint8_t	index;
  uint8_t	strobe;
};

#endif