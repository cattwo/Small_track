#include "client.h"

Client::Client()
    :sockfd(-1)
{
}

int Client::easy_connect(std::string temp_ip,std::string temp_port)
{
    ip = temp_ip;
    port = temp_port;
    const char* ip_char = ip.c_str();
    int port_int = atoi(port.c_str());
    memset(&server_address,0,sizeof (server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, ip_char, &server_address.sin_addr);
    server_address.sin_port = htons(port_int);
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
        return -1;
    if(connect(sockfd, (struct sockaddr*)&server_address, sizeof (server_address)) < 0)
        return -1;
    return 0;
}

int Client::easy_run(int send_fd,int recv_fd)
{
    pollfd fds[2];
    fds[0].fd = send_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN | POLLRDHUP;
    fds[1].revents = 0;
    int ret = pipe(pipefd);
    if(ret == -1)
        return -1;
    if(sockfd == -1)
        return -1;
    while(true)
    {
        /*第三个参数为超时设置，-1表示阻塞至事件发生*/
        ret = poll(fds,2,-1);
        if(ret < 0)
        {
            return -1;
        }
        if(fds[1].revents & POLLRDHUP)
        {
            /*TCP被对方关闭*/
            return -1;
        }
        else if(fds[1].revents & POLLIN)
        {
            ret = splice(fds[1].fd,nullptr,recv_fd,nullptr,32768,SPLICE_F_MORE);
        }

        if(fds[0].revents & POLLIN)
        {
            ret = splice(send_fd,nullptr,sockfd,nullptr,32768,SPLICE_F_MORE);
        }
    }
}

void Client::stop()
{
    close(sockfd);
}

Client::~Client()
{

}
