#include "parser.h"
#include "vm.h"
#include <iostream>
#include <string>

const std::string INPUT = "put 0 add 1 print sleep 1000 goto 3";

int main(int argc, char **argv) {
  std::cout << "Parsing..." << std::endl;
  std::vector<Instruction> result = parse_source_code(INPUT);
  std::cout << "Completed." << std::endl;

  std::cout << "\nExecuting...\n" << std::endl;
  VM vm = VM(result);
  vm.run();

  return 0;
}