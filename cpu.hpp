#ifndef CPU_HXX
#define CPU_HXX
#include "types.hpp"
#include <unordered_map>

#define GENERAL_USE_REGISTERS 32



class Cpu
{
public:
  void print(); //Print all registers

private:
  u32 _generalUse[GENERAL_USE_REGISTERS] = {};
  u32 _PC = 0;
  u64 _HILO = 0;
};

#endif // CPU_HXX