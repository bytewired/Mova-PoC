#ifndef __CLI_H__
#define __CLI_H__

#include "../../shared/shared_utils.h"
#include "hypervisor.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

std::optional<Pipe> find_pipe(int32_t pid) {
  for (const Pipe &pipe : get_named_pipes()) {
    if (pipe.pid == pid)
      return pipe;
  }

  return std::nullopt;
}

void run_cli() {
  std::string input;
  std::vector<byte> data;
  std::unordered_map<int32_t, HypervisorClient *> clients =
      std::unordered_map<int32_t, HypervisorClient *>();

  while (1) {
    std::cout << "> ";
    std::cout.flush();

    std::getline(std::cin, input);

    std::vector<std::string> params = split(input, ' ');
    if (params.size() == 0)
      continue;

    std::string cmd = params.at(0);

    if (cmd == "ps") {
      for (Pipe &pipe : get_named_pipes()) {
        std::cout << "PID: " << pipe.pid
                  << "\n\tRead: " << pipe.pipe_reading_path
                  << "\n\tWrite: " << pipe.pipe_writing_path << std::endl;
      }
    } else if (cmd == "bind") {
      if (params.size() > 1) {
        int32_t pid = std::stoi(params[1]);
        std::optional<Pipe> pipe = find_pipe(pid);
        if (pipe.has_value()) {
          auto client = new HypervisorClient(pipe.value());
          client->bind();
          clients.insert(std::pair(pid, client));
        } else {
          std::cout << "Incorrect PID" << std::endl;
        }
      } else {
        std::cout << "Need to pass PID for this command" << std::endl;
      }
    } else if (cmd == "wstate") {
      if (params.size() > 1) {
        int32_t from_pid = std::stoi(params[1]);
        int32_t to_pid = std::stoi(params[1]);
        auto from_client = clients.find(from_pid);
        auto to_client = clients.find(to_pid);

        if (from_client == clients.end() ||
            from_client->second->saved_state.empty()) {
          std::cout << "Could not find client " << from_pid
                    << " or saved state is absent" << std::endl;
          break;
        }

        if (to_client == clients.end()) {
          std::cout << "Could not find client " << to_pid
                    << ". Call bind before write state" << std::endl;
          break;
        }

        to_client->second->execute(HCommands::WRITE_STATE,
                                   from_client->second->saved_state,
                                   [](std::vector<byte> response) -> void {
                                     std::cout << "State written" << std::endl;
                                   });
      } else {
        std::cout << "Need to pass PID for this command" << std::endl;
      }
    } else if (cmd == "rstate") {
      if (params.size() > 1) {
        int32_t pid = std::stoi(params[1]);
        std::optional<Pipe> pipe = find_pipe(pid);

        if (pipe.has_value()) {
          auto client = clients.find(pid);
          if (client == clients.end()) {
            std::cout << "Not bound to " << pid << std::endl;
          } else {
            client->second->execute(
                HCommands::READ_STATE, std::vector<byte>(),
                [client](std::vector<byte> response) -> void {
                  client->second->saved_state = response;
                });
          }
        } else {
          std::cout << "Incorrect PID" << std::endl;
        }
      } else {
        std::cout << "Need to pass PID for this command" << std::endl;
      }
    } else if (cmd == "unbind") {
      if (params.size() > 1) {
        int32_t pid = std::stoi(params[1]);
        std::optional<Pipe> pipe = find_pipe(pid);

        if (pipe.has_value()) {
          auto client = clients.find(pid);
          if (client == clients.end()) {
            std::cout << "Not bound to " << pid << std::endl;
          } else {
            client->second->terminate();
            clients.erase(pid);
            delete client->second;
          }
        } else {
          std::cout << "Incorrect PID" << std::endl;
        }
      } else {
        std::cout << "Need to pass PID for this command" << std::endl;
      }
    } else if (params[0].empty()) {
      continue;
    } else {
      std::cout << "Unknown command: " << params[0] << std::endl;
    }
  }
}
#endif