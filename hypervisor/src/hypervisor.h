#ifndef __HYPERVISOR_H__
#define __HYPERVISOR_H__

#include "../../shared/hypervisor_communication.h"
#include <chrono>
#include <csignal>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

std::vector<Pipe> get_named_pipes() {
  std::vector<Pipe> pipes = std::vector<Pipe>();
  std::vector<std::string> pids = std::vector<std::string>();

  for (const auto &entry : std::filesystem::directory_iterator("/tmp/")) {
    std::string filename = entry.path().filename();

    if (filename.rfind(std::to_string(PIPE_PREFIX), 0) == 0) {
      std::vector<std::string> params = split(filename, '_');

      if (params.size() != 3)
        continue;

      std::string prefix = params[0];
      std::string pid = params[1];

      if (std::find(pids.begin(), pids.end(), pid) != pids.end())
        continue;

      pids.push_back(pid);

      std::string reading_path = "/tmp/" + prefix + "_" + pid + "_0";
      std::string writing_path = "/tmp/" + prefix + "_" + pid + "_1";

      pipes.push_back(Pipe{.pid = std::stoi(pid),
                           .pipe_reading_path = reading_path,
                           .pipe_writing_path = writing_path});
    }
  }

  return pipes;
}

struct HypervisorClient {
  HypervisorClient(Pipe pipe) : pipe(pipe){};

  std::vector<byte> saved_state;
  void bind();
  void execute(HCommands cmd, std::vector<byte> data,
               std::function<void(std::vector<byte>)> callback);
  void terminate();

private:
  Pipe pipe;
  std::thread t;
  int32_t write_fd;
  int32_t read_fd;
};

void HypervisorClient::bind() {
  read_fd = open(pipe.pipe_reading_path.c_str(), O_RDONLY | O_NONBLOCK);
  write_fd = open(pipe.pipe_writing_path.c_str(), O_WRONLY);

  if (read_fd < 0 || write_fd < 0) {
    std::cout << "Error when opening pipes; read_fd: " << read_fd
              << "; write_fd: " << write_fd << "; errno:" << errno << "\n";
  } else {
    std::cout << "Pipes opened\n";
  }
}

void HypervisorClient::execute(
    HCommands cmd, std::vector<byte> data,
    std::function<void(std::vector<byte>)> callback) {
  switch (cmd) {
  case HCommands::READ_STATE: {
    write_cmd(write_fd, cmd);
    callback(read_from(read_fd));
    break;
  }
  case HCommands::WRITE_STATE: {
    write_cmd(write_fd, cmd);
    write_data(write_fd, data);
    callback(std::vector<byte>());
    break;
  }
  }
}

void HypervisorClient::terminate() {
  pthread_cancel(t.native_handle());
  close(write_fd);
  close(read_fd);
}

#endif