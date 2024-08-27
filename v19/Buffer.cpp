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

void Buffer::append(const char *data,size_t size){
    buf_.append(data, size);
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

