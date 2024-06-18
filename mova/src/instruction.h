#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <stdint.h>
#include <string>

const std::string PUT_KEYWORD = "put";
const std::string ADD_KEYWORD = "add";
const std::string GOTO_KEYWORD = "goto";
const std::string PRINT_KEYWORD = "print";
const std::string SLEEP_KEYWORD = "sleep";

enum OpCode : uint8_t {
  PUT = 0x01,
  ADD = 0x02,
  GOTO = 0x03,
  PRINT = 0x04,
  SLEEP = 0x05
};

union Instruction {
  struct {
    uint8_t opcode;
    uint8_t src_1;
    uint8_t src_2;
    uint8_t dst;
  };

  uint32_t word;
};

Instruction create_instruction_with_int32(OpCode op, int32_t value) {
  return Instruction{.opcode = op,
                     .src_1 = (uint8_t)value,
                     .src_2 = (uint8_t)(value >> 8),
                     .dst = (uint8_t)(value >> 16)};
}

int32_t get_int32_value(const Instruction &instr) {
  return instr.dst << 16 | instr.src_2 << 8 | instr.src_1;
}

#endif