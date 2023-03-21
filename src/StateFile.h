#ifndef _STATE_FILE_H
#define _STATE_FILE_H

#include <stdio.h>

enum StateFileOp {
  StateOp_Load,
  StateOp_Save,
};

class StateFile
{
public:
  StateFile(const char* filename, int op)
  {
    if (op == StateOp_Load) {
      fp = fopen(filename, "rb");
    }
    else {
      fp = fopen(filename, "wb");
    }
  }
  ~StateFile() {}

  template<typename T>
  void Put(T* var)
  {
    fwrite(var, sizeof(T), 1, fp);
  }

  template<typename T>
  void Put(T* var, int size)
  {
    fwrite(var, sizeof(T)*size, 1, fp);
  }

  template<typename T>
  void Get(T* var)
  {
    fread(var, sizeof(T), 1, fp);
  }

  template<typename T>
  void Get(T* var, int size)
  {
    fread(var, sizeof(T)*size, 1, fp);
  }

  void Close()
  {
    fclose(fp);
  }

private:
  FILE* fp;
};

#endif