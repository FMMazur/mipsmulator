#include "cpu.hpp"
#include <iostream>

void Cpu::print()
{
  std::cout << "Registradores:\n";
  for (int i = 0; i < GENERAL_USE_REGISTERS; i++)
  {
    std::cout << i << ": " << _generalUse[i];
  }
  std::cout << "PC: " << _PC;
  std::cout << "HILO: " << _HILO;
}