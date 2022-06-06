#ifndef COMPILER_HXX
#define COMPILER_HXX

#include <string_view>
#include <array>

#include "memory.hpp"
#include "parser.hpp"

class Compiler
{
public:
  Compiler() = delete;
  ~Compiler() {}
  explicit Compiler(std::string_view source);

  std::array<u8, MEMORY_SIZE> compile();

private:
  std::string_view _source;
  Parser _parser;
};

#endif // COMPILER_HXX