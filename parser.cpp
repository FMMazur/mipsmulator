#include "parser.hpp"

#include "utils.hpp"

#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <istream>
#include <vector>

Parser::Parser() : labels{}, _currentMode{Mode::TEXT}
{
  if (Parser::mnemonics.size() == 0)
  {
    mnemonics = {
        {"add", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x20)}},
        {"sub", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x22)}},
        {"addi", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x08)}},
        {"addiu", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x09)}},
        {"mult", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x18)}},
        {"div", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x1a)}},
        {"lw", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x23)}},
        {"lh", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x21)}},
        {"lbu", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x24)}},
        {"sw", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x2b)}},
        {"sh", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x29)}},
        {"sb", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x28)}},
        {"lui", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x0f)}},
        {"mfhi", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x10)}},
        {"mflo", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x12)}},
        {"and", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x24)}},
        {"andi", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x0c)}},
        {"or", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x25)}},
        {"ori", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x0d)}},
        {"slt", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x2a)}},
        {"slti", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x0a)}},
        {"sll", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x00)}},
        {"srl", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x02)}},
        {"beq", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x04)}},
        {"bne", {.format = Instruction::Format::I, .instruction = Instruction::formatI(0x05)}},
        {"j", {.format = Instruction::Format::J, .instruction = Instruction::formatJ(0x02)}},
        {"jr", {.format = Instruction::Format::R, .instruction = Instruction::formatR(0x00, 0x00, 0x08)}},
        {"jal", {.format = Instruction::Format::J, .instruction = Instruction::formatJ(0x03)}},
    };

    registers = {
        {"$zero", 0b00000},
        {"$at", 0b00001},
        {"$v0", 0b00010},
        {"$v1", 0b00011},
        {"$a0", 0b00100},
        {"$a1", 0b00101},
        {"$a2", 0b00110},
        {"$a3", 0b00111},
        {"$t0", 0b01000},
        {"$t1", 0b01001},
        {"$t2", 0b01010},
        {"$t3", 0b01011},
        {"$t4", 0b01100},
        {"$t5", 0b01101},
        {"$t6", 0b01110},
        {"$t7", 0b01111},
        {"$s0", 0b10000},
        {"$s1", 0b10001},
        {"$s2", 0b10010},
        {"$s3", 0b10011},
        {"$s4", 0b10100},
        {"$s5", 0b10101},
        {"$s6", 0b10110},
        {"$s7", 0b10111},
        {"$t8", 0b11000},
        {"$t9", 0b11001},
        {"$k0", 0b11010},
        {"$k1", 0b11011},
        {"$gp", 0b11100},
        {"$sp", 0b11101},
        {"$fp", 0b11110},
        {"$ra", 0b11111},
    };
  }
}

struct JumpInfo
{
  u32 idx;
  Instruction instruction;
  std::string label;

  JumpInfo(u32 idx, Instruction instruction, std::string label) : idx{idx}, instruction{instruction}, label{label} {}
};

Memory
Parser::parse(std::string_view source)
{
  std::stringstream lines{source.data()};
  std::string line;
  std::string text;

  Memory memory;

  std::vector<JumpInfo> jumps;

  u32 idx = 0;

  while (std::getline(lines, line))
  {
    if (line.at(0) == '#')
    {
      continue;
    }

    auto foundComment = line.find('#');
    if (foundComment != std::string::npos)
    {
      line.erase(foundComment);
    }

    std::stringstream ssCurrentLine{line};

    // Pega primeira palavra da linha
    ssCurrentLine >> text;

    // Checa se é a área de texto ou dados
    if (text == ".text")
    {
      _currentMode = Mode::TEXT;
      continue;
    }

    if (text == ".data")
    {
      _currentMode = Mode::DATA;
      continue;
    }

    // Verifica se é uma label
    auto foundLabel = text.find(":");
    if (foundLabel != std::string::npos)
    {
      // TODO: colocar endereço correto para a label
      auto label = text.substr(0, foundLabel);
      labels[label] = idx;

      // se não tiver código para ler, pular para a próxima iteração
      std::string code = text.substr(foundLabel + 1);
      utils::trim(code);

      if (code.size() == 0)
      {
        continue;
      }

      // senão atualizo a stream da linha
      ssCurrentLine.str(code);
    }

    auto commas = std::count(line.begin(), line.end(), ',');

    // Remove as vírgulas
    {
      auto currentString = ssCurrentLine.str().substr(text.size());
      currentString.erase(std::remove(currentString.begin(), currentString.end(), ','), currentString.end());
      ssCurrentLine.str(currentString);
    }

    // Instrução a ser salva
    Instruction instruction;

    switch (_currentMode)
    {
    case Mode::TEXT:
    {
      InstructionType it = Parser::mnemonics[text];

      switch (it.format)
      {
      case Instruction::Format::R:
      {
        std::string rs, rt, rd;
        auto r = it.instruction.r;

        // se duas virgulas 3 argumentos
        if (commas == 2)
        {
          ssCurrentLine >> rd >> rs >> rt;
          instruction = Instruction(r.opcode, registers[rs], registers[rt], registers[rd], r.shamt, r.funct);

          // SLL rd,rt,sa
          if (text == "sll" || text == "srl")
          {
            std::string shamt = rt;
            rt = rs;

            instruction.r.rs = 0x0;
            instruction.r.rt = registers[rt];
            instruction.r.shamt = std::stoul(shamt, nullptr, 16);
          }
        }
        else if (commas == 1)
        {
          // mult/div $rs, $rt
          ssCurrentLine >> rs >> rt;
          instruction = Instruction(r.opcode, registers[rs], registers[rt], 0x0, r.shamt, r.funct);
        }
        else
        {
          // jr $s
          if (text == "jr")
          {
            ssCurrentLine >> rs;
            instruction = Instruction(r.opcode, registers[rs], 0x0, 0x0, r.shamt, r.funct);
          }
          // mflo $d/mfhi $d
          else
          {
            ssCurrentLine >> rd;
            instruction = Instruction(r.opcode, 0x0, 0x0, registers[rd], r.shamt, r.funct);
          }
        }
      }
      break;
      case Instruction::Format::I:
      {
        std::string rs, rt;
        u16 immediate{0};

        auto i = it.instruction.i;

        if (commas == 2)
        {
          ssCurrentLine >> rt >> rs >> immediate;
        }
        else if (commas == 1)
        {
          auto withoutBraces = ssCurrentLine.str();
          utils::replace(withoutBraces, "()", ' ');
          utils::trim(withoutBraces);
          ssCurrentLine.str(withoutBraces);

          ssCurrentLine >> rt >> immediate >> rs;
        }

        instruction = Instruction(i.opcode, registers[rs], registers[rt], immediate);
      }
      break;
      case Instruction::Format::J:
      {
        auto j = it.instruction.j;
        u32 address{0};
        std::string value;
        ssCurrentLine >> value;

        // Se a label já existe
        if (labels.contains(value))
        {
          address = labels[value];
        }
        else
        {
          // se é um hexadecimal
          if (value.starts_with("0x"))
          {
            address = std::stoul(value, nullptr, 16);
          }
          else
          {
            // salva para parsear depois
            jumps.push_back(JumpInfo(
                idx,
                Instruction(it.instruction.j.opcode, 0x0),
                value));

            break;
          }
        }

        instruction = Instruction(j.opcode, address);
      }
      break;
      default:
        break;
      }
      break;
    }
    case Mode::DATA:
      // assert(false && "TODO");
      break;
    default:
      break;
    }

    memory.set(idx / 4, instruction.code);
    idx += 4;
  }

  for (size_t i = 0; i < jumps.size(); i++)
  {
    auto jump = jumps[i];

    try
    {
      jump.instruction.j.address = labels[jump.label] - idx/4;
      memory.set(jump.idx / 4, jump.instruction.code);
    }
    catch (const std::out_of_range &oor)
    {
      std::cerr << oor.what() << std::endl;
    }
  }

  return memory;
}
