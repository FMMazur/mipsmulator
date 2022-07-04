#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>

#include "compiler.hpp"
#include "CPU.hpp"

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    printf("Incorrect usage. Execute with ./mipsimulator <filename>\n");
    return EXIT_FAILURE;
  }

  std::string path{argv[1]};
  std::ifstream inputFile(path);

  if (!inputFile.is_open())
  {
    printf("File not found.");
    return EXIT_FAILURE;
  }

  std::stringstream source;
  source << inputFile.rdbuf();
  inputFile.close();

  Compiler compiler(source.str());
  
  // printf("Compilando...\n");
  auto compiled = compiler.compiled();
  // printf("Compilado.\n");

  Cpu cpu{compiled};
  cpu.run();
  cpu.print();


  return 0;
}
