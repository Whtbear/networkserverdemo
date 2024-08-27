#include "InetAddress.h"

InetAddress::InetAddress(const std::string &ip,uint16_t port){
    addr_.sin_family = AF_INET;//初始化协议簇
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());//服务端ip地址
    addr_.sin_port = htons(port);//监听端口
}

InetAddress::~InetAddress(){

}

const char* InetAddress::getip() const{
    return inet_ntoa(addr_.sin_addr);
}

uint16_t InetAddress::getport() const{
    return ntohs(addr_.sin_port);
}

const sockaddr *InetAddress::getaddr() const{
    return (sockaddr*)&addr_;
}

void InetAddress::setaddr(sockaddr_in clientaddr){
    addr_ = clientaddr;
}
