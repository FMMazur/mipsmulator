#ifndef PARSER_HXX
#define PARSER_HXX

#include <unordered_map>
#include <string>
#include <string_view>

#include "types.hpp"
#include "memory.hpp"

struct InstructionType
{
  Instruction instruction;
  Instruction::Format format;
};

class Parser
{
public:
  Parser();
  ~Parser() {}

  Memory parse(std::string_view source);

  enum class Mode {
    DATA,
    TEXT
  };

private:
  std::unordered_map<std::string, u32> labels;
  Mode _currentMode;

private:
  std::unordered_map<std::string_view, InstructionType> mnemonics;
  std::unordered_map<std::string_view, u8> registers;
};

#endif // PARSER_HXX