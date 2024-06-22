#ifndef __PARSER_H__
#define __PARSER_H__

#include "../../shared/shared_utils.h"
#include "instruction.h"
#include <stdint.h>
#include <string>
#include <vector>

std::vector<Instruction> parse_source_code(const std::string &src) {
  std::vector<Instruction> instrs;
  std::vector<std::string> words = split(src, ' ');

  int32_t i = 0;
  while (i < words.size()) {
    if (words.at(i) == ADD_KEYWORD) {
      int value = std::stoi(words.at(i + 1));

      instrs.push_back(create_instruction_with_int32(ADD, value));

      i += 2;
    } else if (words.at(i) == PUT_KEYWORD) {
      int value = std::stoi(words.at(i + 1));

      instrs.push_back(create_instruction_with_int32(PUT, value));

      i += 2;
    } else if (words.at(i) == GOTO_KEYWORD) {
      int value = std::stoi(words.at(i + 1));

      instrs.push_back(create_instruction_with_int32(GOTO, value));
      i += 2;
    } else if (words.at(i) == PRINT_KEYWORD) {
      instrs.push_back(Instruction{.opcode = PRINT});
      i++;
    } else if (words.at(i) == SLEEP_KEYWORD) {
      int value = std::stoi(words.at(i + 1));

      instrs.push_back(create_instruction_with_int32(SLEEP, value));
      i += 2;
    }
  }

  return instrs;
};

#endif