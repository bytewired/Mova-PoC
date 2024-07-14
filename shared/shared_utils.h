#ifndef __SHARED_UTILS_H__
#define __SHARED_UTILS_H__

#include <iostream>
#include <sstream>
#include <vector>

typedef uint8_t byte;

std::vector<std::string> split(const std::string &str, const char delimeter) {
  std::stringstream stream;
  std::vector<std::string> tokens;
  std::string token;

  while (getline(stream, token, delimeter)) {
    tokens.push_back(token);
  }

  return tokens;
}

void print_bytes_as_hex(byte *array, size_t size) {
  for (uint32_t i = 0; i < size; i++) {
    std::cout << std::hex << (int)array[i] << std::dec << ' ';
  }
  std::cout << std::endl;
}
#endif