#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <memory>
#include <thread>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "node.hpp"


std::string strip(const std::string &input)
{
    auto start_it = input.begin();
    auto end_it = input.rbegin();
    while (std::isspace(*start_it)) {
        ++start_it;
    }
    while (std::isspace(*end_it)) {
        ++end_it;
    }
    return std::string(start_it, end_it.base());
}


int main (int argc, char** argv) {

    int my_id = 0;
    std::unordered_map<int, std::string> servers;

    std::fstream fin;
    std::string line;

    // parse the config file

    std::string filename = "config-" + std::string(argv[1]) + ".txt";

    fin.open(filename);
    while (!fin.eof()) {

        getline(fin, line);
        std::string key = strip(line.substr(0, line.find('=')));
        std::string value = strip(line.substr(line.find('=') + 1));

        if (key.compare("id")==0) 
        {
            my_id = std::stoi(value);
        }
        else if (key.find("server")!=std::string::npos) 
        {
            int id = stoi(strip(key.substr(key.find('.') + 1)));
            std::string addr = strip(value);
            servers.insert({id, addr});
        }
        else
        {
            spdlog::error("Error: invalid key {}", key);
            exit(0);
        }
    }
    
    // create a node
    std::string my_addr = servers[my_id];
    std::string ip = strip(my_addr.substr(0, my_addr.find(':')));
    int port = stoi(strip(my_addr.substr(my_addr.find(':')+1)));

    Node node = Node(my_id, ip, port);

    // start the serving thread
    auto ready_lock = std::make_shared<std::mutex>();
    ready_lock->lock();

    for (auto it = servers.begin(); it != servers.end(); ++it) {
        node.add_server(it->second);
    }

    auto serve = std::thread(&Node::ready, &node, ready_lock);

    ready_lock->lock();

    // try to propose
    // spdlog::info("try to propose something");
    node.broadcast((char*)"hello");
    
    serve.join();
}