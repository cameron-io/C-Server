#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <vector>

constexpr int MAX_EVENTS = 10;
constexpr int MAX_CLIENTS = 10;
constexpr int PORT = 8080;

class Client {
public:
    Client (int clientFd) {
        this->clientFd = clientFd;
    }

    void writeResponse(char * data, int data_len)
    {
        char buffer[1024];

        sprintf(buffer, "HTTP/1.1 200 OK\r\n");
        write(clientFd, buffer, strlen(buffer));

        sprintf(buffer, "Connection: close\r\n");
        write(clientFd, buffer, strlen(buffer));

        sprintf(buffer, "Content-Length: %u\r\n", data_len);
        write(clientFd, buffer, strlen(buffer));

        sprintf(buffer, "Content-Type: %s\r\n", "text/plain");
        write(clientFd, buffer, strlen(buffer));

        sprintf(buffer, "\r\n");
        write(clientFd, buffer, strlen(buffer));

        sprintf(buffer, data);
        write(clientFd, buffer, strlen(buffer));
    }

    // Function to handle client connections in a separate thread
    void handleClient()
    {
        char input_buffer[1024];

        while (true) {
            int bytesRead = read(clientFd, input_buffer, sizeof(input_buffer));
            if (bytesRead <= 0) {
                break;
            }
            char * output_data = "Hello World!";
            int output_data_len = strlen(output_data);
            writeResponse(output_data, output_data_len);
        }

        close(clientFd);
    }
private:
    int clientFd;
};

class Server {
public:
    static int createSocket() {
        int serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd == -1) {
            std::cerr << "Failed to create socket." << std::endl;
            exit(1);
        }
        return serverFd;
    }

    static void bindSocket(int serverFd) {
        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(PORT);
        if (bind(serverFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Failed to bind socket." << std::endl;
            close(serverFd);
            exit(1);
        }
    }

    static void listenSocket(int serverFd) {
        if (listen(serverFd, MAX_CLIENTS) == -1) {
            std::cerr << "Failed to listen." << std::endl;
            close(serverFd);
            exit(1);
        }
    }

    static int acceptConnection(int serverFd) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        return accept(serverFd, (struct sockaddr*)&clientAddress, &clientAddressLength);
    }
};

int main() {
    struct epoll_event event, events[MAX_EVENTS];

    // Create socket
    int serverFd = Server::createSocket();

    // Bind socket to address and port
    Server::bindSocket(serverFd);

    // Listen for incoming connections
    Server::listenSocket(serverFd);

    // Create epoll instance
    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        std::cerr << "Failed to create epoll instance." << std::endl;
        close(serverFd);
        return 1;
    }

    // Add server socket to epoll
    event.events = EPOLLIN;
    event.data.fd = serverFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &event) == -1) {
        std::cerr << "Failed to add server socket to epoll instance." << std::endl;
        close(serverFd);
        close(epollFd);
        return 1;
    }

    std::cout << "Server started. Listening on port " << PORT << std::endl;

    while (true) {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numEvents == -1) {
            std::cerr << "Failed to wait for events." << std::endl;
            break;
        }

        for (int i = 0; i < numEvents; ++i) {
            if (events[i].data.fd == serverFd) {
                // Accept new client connection
                int clientFd = Server::acceptConnection(serverFd);
                if (clientFd == -1) {
                    std::cerr << "Failed to accept client connection." << std::endl;
                    continue;
                }

                // Add client socket to epoll
                event.events = EPOLLIN;
                event.data.fd = clientFd;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
                    std::cerr << "Failed to add client socket to epoll instance." << std::endl;
                    close(clientFd);
                    continue;
                }

                // Create a new thread to handle the client connection
                Client* cl = new Client(clientFd);
                std::thread clientThread([&cl]() { cl->handleClient(); });
                clientThread.detach();
            } else {
                // Handle client data
                int clientFd = events[i].data.fd;
                Client* cl = new Client(clientFd);
                std::thread clientThread([&cl]() { cl->handleClient(); });
                clientThread.detach();
            }
        }
    }

    close(serverFd);
    close(epollFd);
    return 0;
}