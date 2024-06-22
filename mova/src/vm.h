#ifndef __VM_H__
#define __VM_H__

#include "parser.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <stack>
#include <thread>
#include <vector>

struct VM {
  std::vector<Instruction> instrs;
  std::stack<int32_t> stack = std::stack<int>();
  std::atomic_bool is_interruppted = false;
  uint32_t ip = 0;

  VM(std::vector<Instruction> instrs) { this->instrs = instrs; }

  void run();
  void overwrite_state(std::stack<int> stack, uint32_t ip);
};

void VM::run() {
  while (ip < instrs.size()) {
    if (is_interruppted.load()) {
      std::cout << "Process was interrupted" << std::endl;
      while (is_interruppted) {
      } // block execution
    }

    Instruction instr = instrs.at(ip);

    switch (instr.opcode) {
    case PUT:
      stack.push(get_int32_value(instr));
      ip++;
      break;
    case ADD: {
      int op1 = stack.top();
      int op2 = get_int32_value(instr);
      stack.pop();
      stack.push(op1 + op2);
      ip++;
      break;
    }
    case GOTO:
      ip -= get_int32_value(instr);
      break;
    case PRINT:
      // Silent print
      std::cout << stack.top() << std::endl;
      ip++;
      break;
    case SLEEP:
      std::this_thread::sleep_for(
          std::chrono::milliseconds(get_int32_value(instr)));
      ip++;
      break;
    }
  }
}

void VM::overwrite_state(std::stack<int> stack, uint32_t ip) {
  this->stack.swap(stack);
  this->ip = ip;
  this->is_interruppted = false;
}
#endif