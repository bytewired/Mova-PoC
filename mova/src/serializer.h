#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include "instruction.h"
#include "vector"
#include "vm.h"
#include <cassert>

const uint16_t STACK_HEADER = 0x3ACC;
const uint16_t IP_HEADER = 0x101E;

void write_uint16_t(std::vector<uint8_t> &data, uint16_t value) {
  data.push_back(value >> 8);
  data.push_back(value);
}

void write_int32_t(std::vector<uint8_t> &data, int32_t value) {
  data.push_back(value >> 24);
  data.push_back(value >> 16);
  data.push_back(value >> 8);
  data.push_back(value);
}

uint16_t read_uint16_t(const std::vector<uint8_t> &data, uint32_t index) {
  return data[index] << 8 | data[index + 1];
}

int32_t read_int32_t(const std::vector<uint8_t> &data, uint32_t index) {
  return data[index] << 24 | data[index + 1] << 16 | data[index + 2] << 8 |
         data[index + 3];
}

std::vector<uint8_t> serialize(const VM *vm) {
  std::vector<uint8_t> data = std::vector<uint8_t>();

  write_uint16_t(data, STACK_HEADER);
  write_uint16_t(data, vm->stack.size());

  std::stack<int32_t> stack_copy = vm->stack;

  while (stack_copy.size() > 0) {
    write_int32_t(data, stack_copy.top());
    stack_copy.pop();
  }

  write_uint16_t(data, IP_HEADER);
  write_int32_t(data, vm->ip);

  return data;
}

void deserialize(const std::vector<uint8_t> &data, VM *vm) {
  uint32_t i = 0;

  std::stack<int32_t> stack = std::stack<int32_t>();
  uint32_t ip = 0;

  while (i < data.size()) {
    // deserialize stack
    if (i == 0) {
      uint16_t stack_header = read_uint16_t(data, i);
      assert(stack_header == STACK_HEADER);
      i += 2;

      uint16_t stack_size = read_uint16_t(data, i);
      i += 2;

      for (uint16_t j = 0; j < stack_size; j++) {
        stack.push(read_int32_t(data, j));
        i += 4;
      }
    } else {
      uint16_t ip_header = read_uint16_t(data, i);
      assert(ip_header == IP_HEADER);

      ip = read_int32_t(data, i);
      i += 4;
    }
  }

  vm->overwrite_state(stack, ip);
}
#endif