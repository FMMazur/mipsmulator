#ifndef CPU_HXX
#define CPU_HXX
#include "memory.hpp"
#include "types.hpp"
#include <unordered_map>

#define GENERAL_USE_REGISTERS 32

class Cpu
{
public:
  Cpu(Memory &memory) : _memory{memory} {}
  void print(); // Print all registers
  void run();

protected:
  u32 get(u8 idx)
  {
    assert(idx >= 0 && idx < 32);
    return _generalUse[idx];
  }

  void set(u8 idx, u32 value) {
    assert(idx > 0 && idx < 32);
    _generalUse[idx] = value;
  }

  void hilo(u32 hi, u32 lo) {
    _HILO = (((u64)hi) << 32) | lo; 
  }

  void hi(u32 hi) {
    hilo(hi, (u32)_HILO);
  }

  void lo(u32 lo) {
    hilo((u32)(_HILO >> 16), lo); 
  }

private:
  Memory _memory;
  u32 _generalUse[GENERAL_USE_REGISTERS] = {0};
  u32 _PC = 0;
  u64 _HILO = 0;
};

#endif // CPU_HXX