#ifndef __HYPERVISOR_H__
#define __HYPERVISOR_H__

#include "../../shared/hypervisor_communication.h"
#include "serializer.h"
#include "vm.h"
#include <chrono>
#include <csignal>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

struct HypervisorListener {
public:
  HypervisorListener(VM *vm) : vm(vm){};
  void start();

private:
  VM *vm;

  std::vector<byte> listener_callback(std::vector<byte> data);
  void create();
  void listen(const Pipe &pipe);
};

std::vector<byte>
HypervisorListener::listener_callback(std::vector<byte> data) {
  if (!data.empty()) {
    std::cout << "[" << getpid() << "] Received data: ";
    print_bytes_as_hex(data.data(), data.size());
  }

  if (data.size() == 1) {
    switch (data[0]) {
    case HCommands::READ_STATE: {
      vm->is_interruppted = true;
      std::vector<uint8_t> state = serialize(vm);
      vm->is_interruppted = false;
      return state;
    }
    case HCommands::WRITE_STATE: {
      vm->is_interruppted = true;
      deserialize(data, vm);
      vm->is_interruppted = false;
    }
    }
  }

  return std::vector<byte>();
}

void HypervisorListener::create() {
  Pipe pipe = create_pipe();

  listen(pipe);
}

void HypervisorListener::listen(const Pipe &pipe) {
  byte buffer[1024];
  int32_t read_fd;
  int32_t write_fd;

  write_fd = open(pipe.pipe_reading_path.c_str(), O_WRONLY);
  read_fd = open(pipe.pipe_writing_path.c_str(), O_RDONLY | O_NONBLOCK);

  if (read_fd < 0 || write_fd < 0) {
    std::cout << "Error when opening pipes; read_fd: " << read_fd
              << "; write_fd: " << write_fd << "; errno:" << errno << std::endl;
  } else {
    std::cout << "Pipes opened" << std::endl;
  }

  std::vector<byte> read_data = std::vector<byte>();
  while (1) {
    ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer));
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      std::cout << "Received: ";
      print_bytes_as_hex(buffer, bytes_read);
      read_data.insert(read_data.end(), &buffer[0], &buffer[bytes_read - 1]);
    } else if (!read_data.empty()) {
      std::vector<byte> response = listener_callback(read_data);
      if (!response.empty()) {
        const void *response_ptr = response.data();
        write(write_fd, response_ptr, response.size());
        std::cout << "Sent data" << std::endl;
      }

      read_data.clear();
    }
  }

  close(read_fd);
  close(write_fd);
}

void HypervisorListener::start() {
  std::thread t([this] { create(); });
  t.detach();
}

#endif