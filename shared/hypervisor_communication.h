#ifndef __HYPERVISOR_COMMUNICATION_H__
#define __HYPERVISOR_COMMUNICATION_H__

#include "shared_utils.h"
#include <csignal>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

const uint16_t PIPE_PREFIX = 0x616E;

/**
 * Pipe file name for reading: <PIPE_PREFIX>_<PID>_0
 * Pipe file name for writing: <PIPE_PREFIX>_<PID>_1
 */
struct Pipe {
  int32_t pid;
  std::string pipe_reading_path;
  std::string pipe_writing_path;
};

enum HCommands { READ_STATE = 0x12, WRITE_STATE = 0x13 };

Pipe create_pipe() {
  int32_t pid = getpid();
  std::string reading_path =
      "/tmp/" + std::to_string(PIPE_PREFIX) + "_" + std::to_string(pid) + "_0";
  std::string writing_path =
      "/tmp/" + std::to_string(PIPE_PREFIX) + "_" + std::to_string(pid) + "_1";

  mkfifo(reading_path.c_str(), 0666);
  mkfifo(writing_path.c_str(), 0666);

  return Pipe{.pid = pid,
              .pipe_reading_path = reading_path,
              .pipe_writing_path = writing_path};
}

std::vector<byte> read_from(int32_t read_fd) {
  char buffer[1024];
  std::vector<byte> data = std::vector<byte>();
  bool is_start_reading = false;

  while (1) {
    ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer));
    if (bytes_read > 0) {
      data.insert(data.end(), &buffer[0], &buffer[bytes_read]);
      is_start_reading = true;
    } else if (!is_start_reading) {
      // wait for incoming data
      continue;
    } else {
      break;
    }
  }

  std::cout << "Received: ";
  print_bytes_as_hex(data.data(), data.size());
  std::cout << std::endl;

  return data;
}

void write_cmd(int32_t fd, HCommands cmd) {
  byte ccmd = static_cast<byte>(cmd);
  write(fd, &ccmd, sizeof(byte));
}

void write_data(int32_t fd, std::vector<byte> data) {
  write(fd, data.data(), data.size());
}
#endif