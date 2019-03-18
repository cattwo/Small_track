//
// Created by wlwx on 2019/3/17.
//

#include "InetAddress.h"
#include <netdb.h>
#include <memory.h>
#include <arpa/inet.h>

InetAddress::InetAddress()
{
    memset(&addr_,0, sizeof(addr_));
    addr_.sin_family = AF_INET;
}

InetAddress::InetAddress(std::string ip, uint16_t port)
{
    memset(&addr_,0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    port = htons(port);
    addr_.sin_port = port;
    inet_aton(ip.c_str(), &addr_.sin_addr);
}