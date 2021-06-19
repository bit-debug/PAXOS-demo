#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "spdlog/spdlog.h"


class Node {
    public:
        static const int BACKLOG = 3;

        int id;             // id
        std::string addr;   // ip address
        int port;           // port number
        int fd;             // socket fd
        std::string data;   // dummy data
        std::vector<std::string> servers;   // server list
        
        Node (int id, std::string addr, int port) {
            this->id = id;
            this->addr = addr;
            this->port = port;
        }

        // start to serve, accept connection
        void ready (std::shared_ptr<std::mutex> ready_lock);

        // handle request
        void serve (int socket);

        // add a server to the broadcast list
        void add_server (std::string server);

        // send the msg to all servers in the server list
        void broadcast (char* msg);
};


class PAXOS_Node : public Node {
    
};