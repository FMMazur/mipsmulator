#ifndef MEMORY_HXX
#define MEMORY_HXX

#define MEMORY_SIZE 8192
#include "types.hpp"

union Instruction
{
public:
  Instruction() : code(0) {}
  
  Instruction(u32 data) : code(data) {}

  Instruction(u8 opcode, u32 address)
  {
    j.opcode = opcode;
    j.address = address;
  }
  Instruction(u8 opcode, u8 rs, u8 rt, u16 immediate)
  {
    i.opcode = opcode;
    i.rs = rs;
    i.rt = rt;
    i.immediate = immediate;
  }
  Instruction(u8 opcode, u8 rs, u8 rt, u8 rd, u8 shamt, u8 funct)
  {
    r.opcode = opcode;
    r.rs = rs;
    r.rt = rt;
    r.rd = rd;
    r.shamt = shamt;
    r.funct = funct;
  }

  static Instruction formatR(u8 opcode, u8 shamt, u8 funct) { return Instruction(opcode, 0x0, 0x0, 0x0, shamt, funct); }
  static Instruction formatI(u8 opcode) { return Instruction(opcode, 0x0, 0x0, 0x0); };
  static Instruction formatJ(u8 opcode) { return Instruction(opcode, 0x0); };

  enum class Format {
    R,
    I,
    J,
  };

public:
  u32 code;

  struct formatR
  {
    u32 funct : 6;
    u32 shamt : 5;
    u32 rd : 5;
    u32 rt : 5;
    u32 rs : 5;
    u32 opcode : 6;
  } r;

  struct formatI
  {
    u32 immediate : 16;
    u32 rt : 5;
    u32 rs : 5;
    u32 opcode : 6;
  } i;

  struct formatJ
  {
    u32 address : 26;
    u32 opcode : 6;
  } j;
};

class Memory
{
public:
  Instruction get(u32 idx);
  void set(u32 idx, u32 code);

  inline Instruction* content() { return _content; }
  
  inline u32 load(u32 idx) { return reinterpret_cast<u32*>(_content)[idx];}
  inline void store(u32 idx, u8 value) { reinterpret_cast<u8*>(_content)[idx] = value;}

private:
  Instruction _content[MEMORY_SIZE / 4] = {};
};

#endif // MEMORY_HXX