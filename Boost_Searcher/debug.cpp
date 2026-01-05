#include "searcher.hpp"    
#include <cstdio>    
#include <iostream>
#include <cstring>    
#include <string>    
    
const std::string input = "data/raw_html/raw.txt";    
    
int main()    
{    
    ns_searcher::Searcher *search = new ns_searcher::Searcher();    
    search->InitSearcher(input);  //初始化search，创建单例，并构建索引  
    
    std::string query; //自定义一个搜索关键字   
    std::string json_string; //用json串返回给我们   
    char buffer[1024];    
    while(true)    
    {    
        std::cout << "Please Enter You Search Query："; //提示输入   
        fgets(buffer, sizeof(buffer) - 1, stdin);   //读取 
        buffer[strlen(buffer)-1] = 0;    
        query = buffer;    
        search->Search(query, &json_string);  //执行服务，对关键字分词->查找索引->按权重排序->构建json串->保存到json_string->返回给我们                                                                                                                                      
        std::cout << json_string << std::endl;//输出打印    
    }    
    return 0;    
}

