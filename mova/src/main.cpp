#include "hypervisor.h"
#include "parser.h"
#include "serializer.h"
#include "vm.h"
#include <iostream>
#include <string>

const std::string INPUT = "put 0 add 1 print sleep 1000 goto 3";
HypervisorListener *listener = nullptr;

void signal_handler(int32_t signum) {
  if (listener != nullptr)
    listener->stop();

  exit(signum);
}

int main(int argc, char **argv) {
  signal(SIGINT, signal_handler);

  std::cout << "Parsing..." << std::endl;
  std::vector<Instruction> result = parse_source_code(INPUT);
  std::cout << "Completed." << std::endl;

  std::cout << "\nRunning Hypervisor server..." << std::endl;
  VM *vm = new VM(result);

  listener = new HypervisorListener(vm);
  listener->start();

  std::cout << "\nRunning program...\n" << std::endl;
  vm->run();

  return 0;
}