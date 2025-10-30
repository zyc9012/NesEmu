#ifndef _STATE_FILE_H
#define _STATE_FILE_H

#include <cstdio>
#include <cstddef>
#include <string>
#include <vector>
#include <array>

enum class StateFileOp {
  Load,
  Save,
};

class StateFile
{
public:
  StateFile(const std::string& filename, StateFileOp op)
  {
    if (op == StateFileOp::Load) {
      fp = fopen(filename.c_str(), "rb");
    }
    else {
      fp = fopen(filename.c_str(), "wb");
    }
  }
  ~StateFile() = default;

  template<typename T>
  void Put(const T* var)
  {
    fwrite(var, sizeof(T), 1, fp);
  }

  template<typename T>
  void Put(const T* var, size_t size)
  {
    fwrite(var, sizeof(T) * size, 1, fp);
  }
  
  template<typename T, size_t N>
  void Put(const std::array<T, N>& arr)
  {
    fwrite(arr.data(), sizeof(T) * N, 1, fp);
  }
  
  template<typename T>
  void Put(const std::vector<T>& vec)
  {
    fwrite(vec.data(), sizeof(T) * vec.size(), 1, fp);
  }

  template<typename T>
  void Get(T* var)
  {
    fread(var, sizeof(T), 1, fp);
  }

  template<typename T>
  void Get(T* var, size_t size)
  {
    fread(var, sizeof(T) * size, 1, fp);
  }
  
  template<typename T, size_t N>
  void Get(std::array<T, N>& arr)
  {
    fread(arr.data(), sizeof(T) * N, 1, fp);
  }
  
  template<typename T>
  void Get(std::vector<T>& vec)
  {
    fread(vec.data(), sizeof(T) * vec.size(), 1, fp);
  }

  void Close()
  {
    if (fp) {
      fclose(fp);
      fp = nullptr;
    }
  }

private:
  FILE* fp{nullptr};
};

#endif