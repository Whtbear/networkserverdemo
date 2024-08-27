#include "Buffer.h"

/*
class Buffer{
private:
    std::string buf_;   //存放数据

public:
    Buffer();
    ~Buffer();

    void append(const char *data,size_t size);  //数据追加到buf_中
    size_t size();  //返回buf_的大小
    const char *data();     //返回buf_的首地址
    void clear();   //清空buf_。
};
*/

Buffer::Buffer(){

}
Buffer::~Buffer(){

}

void Buffer::erase(size_t pos, size_t nn){
    buf_.erase(pos, nn);
}

void Buffer::append(const char *data,size_t size,int flag){
    if(flag == 0){
        buf_.append(data, size);
    }
    else{
        buf_.append((char*)&size,4);    //处理报文长度（头部）。
        buf_.append(data,size);     //处理报文内容。
    }

}  

size_t Buffer::size(){
    return buf_.size();
}  

const char *Buffer::data(){
    //返回首地址
    return buf_.data();
}   

void Buffer::clear(){
    //清空buf_
    buf_.clear();
}   

