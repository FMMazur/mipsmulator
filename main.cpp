#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>

#include "compiler.hpp"

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
  
  printf("Compilando...\n");
  auto compiled = compiler.compile();

  auto dotFind = path.find_last_of(".");

  std::string outputFilename = argc == 3 ? argv[2] : dotFind != std::string::npos ? path.substr(0, dotFind) + ".bin" : "output.bin";

  std::ofstream outputFile(std::string("./") + outputFilename, std::ios::binary);
  if (!outputFile.is_open()) {
    printf("Can't create file: %s.\n", outputFilename.c_str());
    return EXIT_FAILURE;
  }

  outputFile.write(reinterpret_cast<char*>(compiled.data()), MEMORY_SIZE);

  printf("Salvo em: %s\n", outputFilename.c_str());

  return 0;
}
