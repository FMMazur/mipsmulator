#include "memory.hpp"

Instruction Memory::get(u32 idx)
{
  return _content[idx];
}

void Memory::set(u32 idx, u32 code)
{
  _content[idx].code = code;
}