#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

//定义socket的地址协议类
//封装
class InetAddress{
private:
    sockaddr_in addr_;
public:
    InetAddress(){};
    InetAddress(const std::string &ip,uint16_t port);//构造函数，监听
    InetAddress(const sockaddr_in addr):addr_(addr){};//客户端构造方式
    ~InetAddress();

    void setaddr(sockaddr_in clientaddr);
    const char* getip() const; //返回ip
    uint16_t getport() const; //返回整数端口
    const sockaddr *getaddr() const; //返回addr_成员地址，并进行格式转换
};