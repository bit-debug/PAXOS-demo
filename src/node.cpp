#include "node.hpp"


void Node::ready(std::shared_ptr<std::mutex> ready_lock) {

    // Initialize the TCP/IP socket
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        spdlog::error("Error: socket failed with {}", errno);
        exit(EXIT_FAILURE);
    }
       
    // Forcefully attaching socket to the port
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        spdlog::error("Error: setsockopt failed with {}", errno);
        exit(EXIT_FAILURE);
    }

    // The address to bind and listen
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    int address_length = sizeof(address);

    if (bind(fd, (struct sockaddr *)&address, sizeof(address))<0) {
        spdlog::error("Error: bind failed with {}", errno);
        exit(EXIT_FAILURE);
    }

    if (listen(fd, BACKLOG) < 0) {
        spdlog::error("Error: listen failed with {}", errno);
        exit(EXIT_FAILURE);
    }

    spdlog::info("Server-{} is ready at {}:{}", id, addr, port);
    ready_lock->unlock();

    while (true) {
        int new_socket = accept(fd, (struct sockaddr *)&address, (socklen_t*)&address_length);
        if (new_socket < 0) {
            spdlog::error("Error: accept failed with {}", errno);
            exit(EXIT_FAILURE);
        }

        auto t = std::thread([this, new_socket] { this->serve(new_socket); });
        t.join();
    }
}


void Node::serve (int socket) {
    char buffer[1024] = {0};
    spdlog::info("Server-{} serves the socket", id);
    int bytes_read = read(socket, buffer, 1024);
    spdlog::info("{} bytes read, the message is {}", bytes_read, buffer);
}


void Node::add_server (std::string server) {
    servers.push_back(server);
}


void Node::broadcast (char* msg) {

    std::cout << servers.size() << std::endl;

    for (auto it = servers.begin(); it != servers.end(); ++it) {
        
        std::string address = *it;
        std::string ip = address.substr(0, address.find(':'));
        int port = stoi(address.substr(address.find(':') + 1));
        
        int sock = 0;
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            spdlog::error("Error: broadcast failed at socket creation");
            continue;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        // Convert IPv4 and IPv6 addresses from text to binary form
        if(inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
            spdlog::error("Error: broadcast failed for invalid address: {}", ip);
            continue;
        }
        server_addr.sin_port = htons(port);
   
        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            spdlog::error("Error: broadcast failed at connecting {}", address);
            continue;
        }

        send(sock, msg, strlen(msg), 0);
        spdlog::info("Message sent to {}", address);
    }
}