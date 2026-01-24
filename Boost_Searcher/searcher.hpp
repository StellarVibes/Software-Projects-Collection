#pragma once  
#include "index.hpp"
#include "util.hpp"
#include <algorithm>
#include <jsoncpp/json/json.h>
#include <vector>
#include <string>
namespace ns_searcher
{

    // 将word变成数组，因为搜索的关键字中可能有多个关键字对应一个文档
    // 为了使文档只出现一次，我们将所以倒排拉链的文档都去重
    // 相同文档的将权重加起来，把映射这个文档的关键字填写到数组里面

    //该结构体是用来对重复文档去重的结点结构
    struct InvertedElemPrint
    {
        uint64_t doc_id;  //文档ID
        int weight;       //重复文档的权重之和
        std::vector<std::string> words;//关键字的集合，我们之前的倒排拉链节点只能保存一个关键字
        InvertedElemPrint():doc_id(0), weight(0){}
    };


    class Searcher
    {
    private:
        ns_index::Index *index; //供系统进行查找的索引
    public:
        Searcher(){}
        ~Searcher(){}
    public:
        void InitSearcher(const std::string &input)
        {
            // 获取或者创建index对象（单例）
            index = ns_index::Index::GetInstance();  
            //std::cout<< "获取index单例成功...."<<std::endl;
            LOG(NORMAL , "获取index单例成功....");
            // 根据index对象建立索引
            index->BuildIndex(input);
            //std::cout<< "建立正排和倒排索引成功...."<<std::endl;
            LOG(NORMAL , "建立正排和倒排索引成功....");
        }

        //query: 搜索关键字
        //json_string: 返回给用户浏览器的搜索结果
        void Search(const std::string &query, std::string *json_string)
        {
            //1.[分词]:对搜索关键字query在服务端也要分词，然后查找index
            std::vector<std::string> words; //用一个数组存储分词的结果   
            ns_util::JiebaUtil::CutString(query, &words);//分词操作


            //2.[触发]:根据分词的各个词进行index查找
            //ns_index::InvertedList inverted_list_all;
            std::vector<InvertedElemPrint> inverted_list_all; //用vector来保存

            std::unordered_map<uint64_t, InvertedElemPrint> tokens_map;//用来去重

            for(std::string word : words)//遍历分词后的每个词
            {
                boost::to_lower(word);//忽略大小写
                // 通过word 关键字 获取 一个数组形式的 倒排拉链
                ns_index::InvertedList* inverted_list = index->GetInvertedList(word);//获取倒排拉链
                if(nullptr == inverted_list)
                {
                    continue;
                }

                //inverted_list_all.insert(inverted_list_all.end() , inverted_list->begin() , inverted_list->end());
                //遍历获取上来的倒排拉链
                for(const auto &elem : *inverted_list)
                {
                    auto &item = tokens_map[elem.doc_id];//插入到tokens_map中，key值如果相同，这修改value中的值
                    item.doc_id = elem.doc_id;
                    item.weight += elem.weight;//如果是重复文档，key不变，value中的权重累加
                    item.words.push_back(elem.word);//如果树重复文档，关键字会被放到vector中保存
                }
            }

            //遍历tokens_map，将它存放到新的倒排拉链集合中（这部分数据就不存在重复文档了）
            for(const auto &item : tokens_map)                                                                                                                                        
            {
                inverted_list_all.push_back(std::move(item.second));
            }

            //3.[合并排序]:汇总查找结果，按照相关性（权重weight）降序排序
            std::sort(inverted_list_all.begin(), inverted_list_all.end(),\
         [](const InvertedElemPrint &e1, const InvertedElemPrint &e2)
         {return e1.weight > e2.weight;});


            //4.[构建]:将排好序的结果，生成json串 —— jsoncpp
            Json::Value root;    
            for(auto &item : inverted_list_all)    
            {    
                ns_index::DocInfo *doc = index->GetForwardIndex(item.doc_id);    
                if(nullptr == doc)    
                {    
                continue;    
                }   
 
                Json::Value elem;    
                elem["title"] = doc->title;    
                elem["desc"] = GetDesc(doc->content, item.words[0]); //content是文档去标签后的结果，但不是我们想要的，我们要的是一部分                                                     
                elem["url"] = doc->url;    
    
                //调式    
                //elem["id"] = (int)item.doc_id;    
                //elem["weight"] = item.weight;    
    
                root.append(elem);    
            }    
            Json::StyledWriter writer; //方便调试    
            //Json::FastWriter writer;//调式没问题后使用这个    
            *json_string = writer.write(root);

        }

        std::string GetDesc(const std::string &html_content, const std::string &word)
        {
            //找到word(关键字)在html_content中首次出现的位置
            //然后往前找50个字节(如果往前不足50字节，就从begin开始)
            //往后找100个字节(如果往后不足100字节，就找到end即可)
            //截取出这部分内容
            
            const int prev_step = 50;
            const int next_step = 100;
            //1.找到首次出现
            auto iter = std::search(html_content.begin(), html_content.end(), word.begin(), word.end(), [](int x, int y){
            return (std::tolower(x) == std::tolower(y));
            });
 
            if(iter == html_content.end())
            {
                return "None1";
            }
            int pos = std::distance(html_content.begin(), iter);
            
            //2.获取start和end位置
            int start = 0;
            int end = html_content.size() - 1;
            //如果之前有50个字符，就更新开始位置
            if(pos > start + prev_step) start = pos - prev_step;
            if(pos < end - next_step) end = pos + next_step;
 
            //3.截取子串，然后返回
            if(start >= end) return "None2";
            std::string desc = html_content.substr(start,end - start);
            desc += "...";
            return desc;
        }
 
    };
}
