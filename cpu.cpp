#include "cpu.hpp"
#include <iostream>

void Cpu::print()
{
  printf("Registradores:\n");
  for (int i = 0; i < GENERAL_USE_REGISTERS; i++)
  {
    printf("%i: 0x%.8x\n", i, _generalUse[i]);
  }
  printf("PC: 0x%.8x\n", _PC);
  printf("HILO: 0x%.16llx\n", _HILO);
}

void Cpu::run()
{
  _PC = 0;
  set(29, MEMORY_SIZE); // $sp

  bool running = true;

  while (running)
  {
    auto it = _memory.get(_PC / 4);
    auto opcode = it.r.opcode;

    switch (opcode)
    {
    case 0x00:
    {
      switch (it.r.funct)
      {
      case 0x00: // SLL
        set(it.r.rd, get(it.r.rt) << it.r.shamt);
        break;
      case 0x02: // SRL
        set(it.r.rd, get(it.r.rt) >> it.r.shamt);
        break;
      case 0x08: // JR
        _PC = get(it.r.rs);
        break;
      case 0x10: // mfhi
        set(it.r.rd, _HILO >> 32);
        break;
      case 0x12: // mflo
        set(it.r.rd, (u32)_HILO);
        break;
      case 0x18: // mult
      {
        u64 mult = ((u64)get(it.r.rs)) * get(it.r.rt);

        hilo(mult >> 32, (u32)mult);
      }
      break;
      case 0x20: // add
        set(it.r.rd, get(it.r.rs) + get(it.r.rt));
        break;
      case 0x22: // sub
        set(it.r.rd, get(it.r.rs) - get(it.r.rt));
        break;
      case 0x24: // and
        set(it.r.rd, get(it.r.rs) & get(it.r.rt));
        break;
      case 0x25: // or
        set(it.r.rd, get(it.r.rs) | get(it.r.rt));
        break;
      case 0x0c: // syscall
        if (get(2) == 10)
        {
          running = false;
        }
        break;
      case 0x1a: // div
        lo(get(it.r.rs) / get(it.r.rt));
        hi(get(it.r.rs) % get(it.r.rt));
        break;
      case 0x2a: // slt
        set(it.r.rd, get(it.r.rs) < get(it.r.rt) ? 1 : 0);
        break;
      default:
        printf("formatR - Funct not implemented\n");
        break;
      }
      break;
    }

    case 0x02: // j
      _PC = it.j.address;
      continue;
    case 0x03: // jal
      set(31, _PC + 4);
      _PC = it.j.address;
      continue;
    case 0x04: // beq
      if (get(it.i.rs) == get(it.i.rt))
        _PC = it.i.immediate;
      break;

    case 0x05: // bne
      if (get(it.i.rs) != get(it.i.rt))
        _PC = it.i.immediate;
      break;

    case 0x08: // addi
    case 0x09: // addiu
      set(it.i.rt, (int)get(it.i.rs) + (short)it.i.immediate);
      break;

    case 0x0a: // slti
      set(it.i.rt, get(it.i.rs) < it.i.immediate ? 1 : 0);
      break;
    case 0x0c: // addi
      set(it.i.rt, get(it.i.rs) + it.i.immediate);
      break;
    case 0x0d: // ori
      set(it.i.rt, get(it.i.rs) | it.i.immediate);
      break;

    case 0x0f: // lui
      set(it.i.rt, it.i.immediate << 16);
      break;

    case 0x21: // lh
      set(it.i.rt, _memory.load((get(it.i.rs) + it.i.immediate) / 4) & 0xFFFF);
      break;
    case 0x23: // lw
      set(it.i.rt, _memory.load((get(it.i.rs) + it.i.immediate) / 4));
      break;
    case 0x24: // lbu
      set(it.i.rt, _memory.load((get(it.i.rs) + it.i.immediate) / 4) & 0xFF);
      break;

    case 0x28: // sh
      _memory.set((get(it.i.rs) + it.i.immediate)/4, get(it.i.rt) & 0xFFFF);
      break;
    case 0x29: // sb
      _memory.set((get(it.i.rs) + it.i.immediate)/4, get(it.i.rt) & 0xFF);
      break;

    case 0x2b: // sw
      _memory.set((get(it.i.rs) + it.i.immediate)/4, get(it.i.rt));
      break;

    default:
      break;
    }

    _PC += 4;
  }
}