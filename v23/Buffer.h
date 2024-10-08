#pragma once
#include <string>
#include <iostream>

class Buffer{
private:
    std::string buf_;   //存放数据

public:
    Buffer();
    ~Buffer();

    void append(const char *data,size_t size);  //数据追加到buf_中
    void erase(size_t pos, size_t nn);  //从buf_的pos开始，删除nn个字节，pos从0开始
    size_t size();  //返回buf_的大小
    const char *data();     //返回buf_的首地址
    void clear();   //清空buf_。
};