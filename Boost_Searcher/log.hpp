#pragma once     
#include <iostream>    
#include <string>    
#include <ctime>    
    
#define NORMAL 1   //正常的                                                                                                                                                                     
#define WARNING 2  //错误的     
#define DEBUG 3    //bug    
#define FATAL 4    //致命的   

   
#define LOG(LEVEL, MESSAGE) log(#LEVEL, MESSAGE, __FILE__, __LINE__)   
     
// 在那个文件内部，第几行出现了什么样的错误     
void log(std::string level, std::string message, std::string file, int line)    
{    
    std::cout << "[" << level << "]" << "[" << time(nullptr) << "]" << "[" << message << "]" << "[" << file << " : " << line << "]" << std::endl;    
} 
/*
简单说明：   
    我们用宏来实现日志功能，其中LEVEL表明的是等级（有四种），
    这里的#LEVEL的作用是：把一个宏参数变成对应的字符串（直接替换）
C语言中的预定义符号：
    __FILE__：进行编译的源文件
    __LINE__：文件的当前行号
补充几个：
    __DATE__：文件被编译的日期
    __TIME__：文件被编译的时间
    __FUNCTION__：进行编译的函数
*/