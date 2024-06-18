#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include <instruction.h>
#include <vector>
#include <vm.h>

const uint16_t STACK_HEADER = 0x3ACC;
const uint16_t IP_HEADER = 0x101E;


std::vector<unsigned char> serialize(const VM &vm) {
    std::vector<unsigned char> data = std::vector<unsigned char>();

    data.push_back(STACK_HEADER);
    data.push_back(vm.stack.size());

    std::stack<int32_t> stack_copy = vm.stack;

    return data;
}

#endif