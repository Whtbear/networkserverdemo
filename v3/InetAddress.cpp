#include "InetAddress.h"

/*方便查看
class InetAddress{
private:
    sockaddr_in addr_;
public:
    InetAddress(const std::string &ip,uint16_t port);//构造函数，监听
    InetAddress(const sockaddr_in addr):addr_(addr){};//客户端构造方式
    ~InetAddress();

    const char* getip() const; //返回ip
    uint16_t getport() const; //返回整数端口
    const sockaddr *getaddr() const; //返回addr_成员地址，并进行格式转换
}
*/
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
