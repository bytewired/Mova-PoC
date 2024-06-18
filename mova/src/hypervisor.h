#ifndef __HYPERVISOR_H__
#define __HYPERVISOR_H__

#include "serializer.h"
#include "vm.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

void start_hypervisor_server(VM *vm) {
  int32_t server_fd;

  if ((server_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Error while opening socket: " << server_fd;
    exit(EXIT_FAILURE);
  }

  sockaddr_in address;
  address.sin_family = AF_LOCAL;
  address.sin_port = htons(getpid() ^ 32456);
  address.sin_addr.s_addr = INADDR_ANY;

  bind(server_fd, (sockaddr *)&address, sizeof(address));

  while (1) {
    if (listen(server_fd, 1) != 0)
      continue;
    std::cout << "Client connected. Receiving messages..." << std::endl;

    int32_t client_socket = accept(server_fd, nullptr, nullptr);
    std::vector<uint8_t> received_data = std::vector<uint8_t>(1024);

    uint8_t buffer[1024] = {0};
    int32_t response_size = 0;

    do {
      response_size = recv(client_socket, buffer, sizeof(buffer), 0);
      received_data.insert(received_data.end(), &buffer[0],
                           &buffer[response_size]);
    } while (response_size > 0);

    if (!received_data.empty()) {
      vm->is_interruppted = true;
      deserialize(received_data, *vm);
      vm->is_interruppted = false;
    }
  }
}

void run_hypervisor_server(VM *vm) {
  std::thread t(start_hypervisor_server, vm);
}

#endif