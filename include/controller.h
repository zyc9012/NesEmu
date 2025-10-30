#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <cstdint>
#include <array>

enum class ControllerButton : uint8_t {
  A = 0,
  B,
  Select,
  Start,
  Up,
  Down,
  Left,
  Right,
};

class Controller
{
public:
  Controller() = default;
  ~Controller() = default;
  
  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;
  Controller(Controller&&) = default;
  Controller& operator=(Controller&&) = default;
  
  void SetButtons(const std::array<bool, 8>& buttons);
  std::array<bool, 8> GetButtons() const { return buttons; }
  uint8_t Read();
  void Write(uint8_t value);

  std::array<bool, 8> buttons{};
  uint8_t index{0};
  uint8_t strobe{0};
};

#endif