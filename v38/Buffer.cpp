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

Buffer::Buffer(uint16_t sep):sep_(sep){

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
        if(sep_ == 0){
            buf_.append(data,size);
        }
        else if(sep_ == 1){
            buf_.append((char*)&size,4);    //处理报文长度（头部）。
            buf_.append(data,size);     //处理报文内容。
        }
        else{
            buf_.append(data,size);
            buf_.append("\r\n\r\n");

        }
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

bool Buffer::pickmessage(std::string &ss){
    if(buf_.size() == 0) return false;

    if(sep_ == 0){
        ss = buf_;
        buf_.clear();
    }
    else if(sep_ == 1){
        int len;
        memcpy(&len,buf_.data(),4);
        if(buf_.size() < len + 4) return false;//报文内容不完整
        
        ss = buf_.substr(4,len);
        buf_.erase(0, len + 4);
    }
    else{
        //"\r\n\r\n"分隔方式的存放
    }

    return true;
}
