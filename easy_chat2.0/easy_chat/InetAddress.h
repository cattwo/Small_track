//
// Created by wlwx on 2019/3/17.
//

#ifndef EASY_CHAT_INETADDRESS_H
#define EASY_CHAT_INETADDRESS_H

#include <netinet/in.h>
#include <string>

class InetAddress
{
public:
    explicit InetAddress();
    explicit InetAddress(std::string ip, uint16_t port);

    explicit InetAddress(const struct sockaddr_in& addr)
            :addr_(addr)
    {}

    ~InetAddress();

    struct sockaddr_in* getAddr()  { return &addr_;}


private:
    struct sockaddr_in addr_;

};


#endif //EASY_CHAT_INETADDRESS_H
