#include "hypervisor.h"
#include "parser.h"
#include "serializer.h"
#include "vm.h"
#include <iostream>
#include <string>

const std::string INPUT = "put 0 add 1 print sleep 1000 goto 3";

int main(int argc, char **argv) {
  std::cout << "Parsing..." << std::endl;
  std::vector<Instruction> result = parse_source_code(INPUT);
  std::cout << "Completed." << std::endl;

  std::cout << "\nRunning Hypervisor server..." << std::endl;
  VM *vm = new VM(result);
  run_hypervisor_server(vm);

  std::cout << "\nRunning program...\n" << std::endl;
  vm->run();

  return 0;
}