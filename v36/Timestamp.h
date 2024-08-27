#pragma once

#include <iostream>
#include <string>

//时间戳类
class Timestamp{
private:
    time_t secsinceepoch_;  //整数表示的时间 1970--至今秒

public:
    Timestamp();    //当前时间初始化对象
    Timestamp(int64_t secsinceepoch);   //用一个整数表示的时间初始化对象

    static Timestamp now(); //返回当前时间的Timestamp对象

    time_t toint() const;   //返回整数表示时间
    std::string tostring() const;   //返回字符串表示的时间，格式： yyyy-mm-dd hh:mi:ss

};