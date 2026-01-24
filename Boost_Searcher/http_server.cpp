#include "httplib.h"
#include "searcher.hpp"    
    
const std::string input = "data/raw_html/raw.txt";    
const std::string root_path = "./wwwroot";    
    
int main()    
{    
    ns_searcher::Searcher search;    
    search.InitSearcher(input); 
   
    //创建一个Server对象，本质就是搭建服务端
    httplib::Server svr;   
 
    //访问首页
    svr.set_base_dir(root_path.c_str()); 
  
     // 这里注册用于处理 get 请求的函数，当收到对应的get请求时（请求s时），程序会执行对应的函数（也就是lambda表达式）
    svr.Get("/s", [&search](const httplib::Request &req, httplib::Response &rsp){
            //has_param：这个函数用来检测用户的请求中是否有搜索关键字，参数中的word就是给用户关键字取的名字（类似word=split）    
            if(!req.has_param("word")){    
                rsp.set_content("必须要有搜索关键字!", "text/plain; charset=utf-8");    
                return;    
            }    
 
            //获取用户输入的关键字
            std::string word = req.get_param_value("word");    
            //std::cout << "用户在搜索：" << word << std::endl;
            LOG(NORMAL , "用户在搜索：" + word);    
            
            //根据关键字，构建json串
            std::string json_string;    
            search.Search(word, &json_string);
 
            //设置 get "s" 请求返回的内容，返回的是根据关键字，构建json串内容
            rsp.set_content(json_string, "application/json");       
            });    
 
    //std::cout << "服务器启动成功......" << std::endl; 
    LOG(NORMAL , "服务器启动成功......");    
    // 绑定端口（8080），启动监听（0.0.0.0表示监听任意端口）
    svr.listen("0.0.0.0", 8080);                                                                                                                                                      
    return 0;    
}