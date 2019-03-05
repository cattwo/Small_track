#ifndef CLIENT_H
#define CLIENT_H
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <string>
#include <time.h>

class Client
{
public:
    Client();
    ~Client();
    void stop();
    int easy_connect(std::string, std::string);
    int easy_run(int, int);
private:
    std::string ip;
    std::string port;
    struct sockaddr_in server_address;
    int sockfd;
    int pipefd[2];
};

#endif // CLIENT_H
