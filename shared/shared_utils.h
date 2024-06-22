#ifndef __SHARED_UTILS_H__
#define __SHARED_UTILS_H__

#include <iostream>
#include <vector>

typedef uint8_t byte;

std::vector<std::string> split(const std::string &str, const char delimeter) {
  std::vector<std::string> words;
  std::string acc;

  for (char c : str) {
    if (c == delimeter) {
      words.push_back(acc);
      acc = "";
    } else {
      acc += c;
    }
  }

  words.push_back(acc);

  return words;
}

void print_bytes_as_hex(byte *array, size_t size) {
  for (uint32_t i = 0; i < size; i++) {
    std::cout << std::hex << (int)array[i] << std::dec << ' ';
  }
  std::cout << std::endl;
}
#endif