#include "compiler.hpp"

Compiler::Compiler(std::string_view source) : _source(source), _parser() {}

std::array<u8, MEMORY_SIZE> Compiler::compile() {
  Memory parsed = _parser.parse(_source);
  std::array<u8, MEMORY_SIZE> compiled{0};

  Instruction* content = parsed.content();

  for (u32 i = 0; i < MEMORY_SIZE / 4; i++) {
    u32 idx = i * 4;

    compiled[idx]     = static_cast<u8>(content[i].code >> 24);
    compiled[idx + 1] = static_cast<u8>(content[i].code >> 16);
    compiled[idx + 2] = static_cast<u8>(content[i].code >> 8);
    compiled[idx + 3] = static_cast<u8>(content[i].code & 0xFF);
  }

  return compiled;
}

Memory Compiler::compiled() {
  return _parser.parse(_source);
}