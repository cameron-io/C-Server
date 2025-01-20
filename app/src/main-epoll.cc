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
constexpr char* base_path = "static";

const char *get_content_type(const char* path) {
    const char *last_dot = strrchr(path, '.');
    if (last_dot) {
        if (strcmp(last_dot, ".css") == 0) return "text/css";
        if (strcmp(last_dot, ".csv") == 0) return "text/csv";
        if (strcmp(last_dot, ".gif") == 0) return "image/gif";
        if (strcmp(last_dot, ".htm") == 0) return "text/html";
        if (strcmp(last_dot, ".html") == 0) return "text/html";
        if (strcmp(last_dot, ".ico") == 0) return "image/x-icon";
        if (strcmp(last_dot, ".jpeg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".jpg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".js") == 0) return "application/javascript";
        if (strcmp(last_dot, ".json") == 0) return "application/json";
        if (strcmp(last_dot, ".png") == 0) return "image/png";
        if (strcmp(last_dot, ".pdf") == 0) return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0) return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0) return "text/plain";
    }

    return "application/octet-stream";
}

void send_400(int clientFd)
{
    const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 11\r\n\r\nBad Request";
    write(clientFd, c400, strlen(c400));
}

void send_404(int clientFd)
{
    const char *c404 = "HTTP/1.1 404 Not Found\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 9\r\n\r\nNot Found";
    write(clientFd, c404, strlen(c404));
}

void serve_resource(
    int clientFd,
    const char *path,
    const char *base_path)
{
    if (strcmp(path, "/") == 0)
        path = "/index.html";

    if (strlen(path) > 100)
    {
        send_400(clientFd);
        return;
    }

    if (strstr(path, ".."))
    {
        send_404(clientFd);
        return;
    }

    char full_path[128];
    sprintf(full_path, "%s%s", base_path, path);

    FILE *fp = fopen(full_path, "rb");

    if (!fp)
    {
        send_404(clientFd);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    size_t cl = ftell(fp);
    rewind(fp);

    const char *ct = get_content_type(full_path);

#define BSIZE 1024
    char buffer[BSIZE];

    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Connection: close\r\n");
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Content-Length: %u\r\n", (unsigned int)cl);
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Content-Type: %s\r\n", ct);
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "\r\n");
    write(clientFd, buffer, strlen(buffer));

    int r = fread(buffer, 1, BSIZE, fp);
    while (r)
    {
        write(clientFd, buffer, r);
        r = fread(buffer, 1, BSIZE, fp);
    }

    fclose(fp);
}

void handle_request(int clientFd, char* request) {
    if (strncmp("GET /", request, 5)) {
        send_400(clientFd);
    } else {
        char *path = request + 4;
        char *end_path = strstr(path, " ");
        if (!end_path) {
            send_400(clientFd);
        } else {
            *end_path = 0;
            serve_resource(clientFd, path, base_path);
        }
    }
}

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
            handle_request(clientFd, input_buffer);
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