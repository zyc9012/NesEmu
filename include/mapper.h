#ifndef _MAPPER_H
#define _MAPPER_H

#include <stdint.h>
class StateFile;

class Mapper
{
public:
  //Mapper();
  //~Mapper();

  virtual uint8_t Read(uint16_t address) = 0;
  virtual void Write(uint16_t address, uint8_t value) = 0;
  virtual void Step() = 0;
  virtual bool Save(StateFile*) = 0;
  virtual bool Load(StateFile*) = 0;
};

#endif