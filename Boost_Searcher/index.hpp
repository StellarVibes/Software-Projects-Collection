#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <mutex>
#include "util.hpp"
#include "log.hpp"

namespace ns_index
{
    struct DocInfo //文档信息节点
    {
        std::string title;    //文档的标题
        std::string content;  //文档对应的去标签后的内容
        std::string url;      //官网文档的url
        uint64_t doc_id;      //文档的ID
    };
 
    // 一个【关键字】可能出现在 无数个 【文档】中 ，我们需要根据权重判断 文档的重要顺序
    // 注意：只是一个关键字和文档的关系，我们会存在一个关键字 对应多个文档   -- 需要后面的 倒排拉链
    struct InvertedElem //倒排对应的节点
    {
        uint64_t doc_id;      //文档ID
        std::string word;     //关键字（通过关键字可以找到对应的ID）
        int weight;           //权重---根据权重对文档进行排序展示
    };

    // 倒排拉链  -- 一个关键字 可能存在于多个文档中，所以一个关键字对应了一组文档
    typedef std::vector<InvertedElem> InvertedList;
 
    class Index
    {
    private:
        // 正排索引的数据结构采用数组，数组下标就是天然的文档ID
        // 每一个数组里面存放一个 文档信息
        std::vector<DocInfo> forward_index; //正排索引

        // 一个【关键字】可能出现在 无数个 【文档】中 ，我们需要根据权重判断 文档的重要顺序
        //倒排索引一定是一个关键字和一组（或者一个）InvertedElem对应[关键字和倒排拉链的映射关系]
        std::unordered_map<std::string, InvertedList> inverted_index;

    // 将 Index 转变成单例模式
    private:
        Index(){} //这个一定要有函数体，不能delete
        Index(const Index&) = delete;  // 拷贝构造
        Index& operator = (const Index&) = delete; // 赋值重载
        static Index* instance;
        static std::mutex mtx;//C++互斥锁，防止多线程获取单例存在的线程安全问题

    public:
        ~Index(){}
 
    public:
        //获取index单例
        static Index* GetInstance()
        {
            // 这样的【单例】 可能在多线程中产生 线程安全问题，需要进行加锁
            if(nullptr == instance)// 双重判定空指针, 降低锁冲突的概率, 提高性能
            {
                mtx.lock();//加锁
                if(nullptr == instance)
                {
                    instance = new Index();//获取单例
                }
                mtx.unlock();//解锁
            }
            return instance;
        }

        //根据doc_id找到正排索引对应doc_id的文档内容
        DocInfo* GetForwardIndex(uint64_t doc_id)
        {
            //如果这个doc_id已经大于正排索引的元素个数，则索引失败
            if(doc_id >= forward_index.size())  // 相当于 越界
            {                                                                                                                                                         
                std::cout << "doc_id out range, error!" << std::endl;
                return nullptr;
            }
            return &forward_index[doc_id];//否则返回相应doc_id的文档内容
        }
        
        //根据倒排索引的关键字word，获得倒排拉链
        InvertedList* GetInvertedList(const std::string &word)
        {
            // word关键字不是在 unordered_map 中，直接去里面找对应的倒排拉链即可
            auto iter = inverted_index.find(word);
            if(iter == inverted_index.end())  // 判断是否越界
            {
                std::cerr << " have no InvertedList" << std::endl;
                return nullptr;
            }
            // 返回 unordered_map 中的第二个元素--- 倒排拉链
            return &(iter->second);
        }
        
        //根据去标签，格式化后的文档，构建正排和倒排索引                                                                                                              
        //将数据源的路径：data/raw_html/raw.txt传给input即可，这个函数用来构建索引
        bool BuildIndex(const std::string &input)
        {
            // 要构建索引，肯定先把我们之前处理好的 raw.txt 打开，按行处理（每一行就是一个.html 文件）
            // 在上面SaveHtml函数中，我们是以二进制的方式进行保存的，那么读取的时候也要按照二进制的方式读取，读取失败给出提示
            std::ifstream in(input, std::ios::in | std::ios::binary);  // 读取input(raw.txt) 
            if(!in.is_open()) 
            {
                std::cerr << "sory, " << input << " open error" << std::endl;
                return false;
            }
 
            std::string line;
            int count = 0;
            while(std::getline(in, line))
            {
                DocInfo* doc = BuildForwardIndex(line);//构建正排索引
 
                if(nullptr == doc)
                {
                    std::cerr << "build " << line << " error" << std::endl;
                    continue;
                }
 
                BuildInvertedIndex(*doc);//有了正排索引才能构建倒排索引
                count++;    
                if(count % 50 == 0)    
                {    
                    //std::cout << "当前已经建立的索引文档：" << count << "个" << std::endl;
                    LOG(NORMAL , "当前的已经建立的索引文档 : " + std::to_string(count));     
                }
            }
            return true;
        }

    private:
        // 构建正排索引 将拿到的一行html文件传输进来，进行解析
        // 构建的正排索引，就是填充一个 DocInfo这个数据结构 ，然后将 DocInfo 插入 正排索引的 vector中即可 
        DocInfo* BuildForwardIndex(const std::string& line)
        {
            // 1. 解析 line ，字符串的切分  分为 DocInfo 中的结构
            // 1. line -> 3 个 string (title , content , url)
            std::vector<std::string> results;
            std::string sep = "\3"; //行内分隔符
            ns_util::StringUtil::Split(line, &results, sep);//字符串切分 
            if(results.size() != 3)                                             
            {                                                                   
                return nullptr;                                                 
            }  
            // 2. 字符串填充到 DocInfo 中
            DocInfo doc;                                                        
            doc.title = results[0];                                             
            doc.content = results[1];                                           
            doc.url = results[2];                                               
            doc.doc_id = forward_index.size(); //先进行保存id，在插入，对应的id就是当前doc在vector中的下标
            // 3. 插入到正排索引的 vector 中
            forward_index.push_back(std::move(doc)); //使用move可以减少拷贝带来的效率降低

            return &forward_index.back();                                       
        }

        // 构建倒排索引
        bool BuildInvertedIndex(const DocInfo &doc)
        {
            // DocInfo (title , content , url , doc_id)
            // word(关键字) -> 倒排拉链

            //词频统计结构体--表示一个节点 
            struct word_cnt    
            {    
                int title_cnt;    
                int content_cnt;    
                word_cnt():title_cnt(0), content_cnt(0){}    
            };    
            // 关键字和词频结构体的映射
            std::unordered_map<std::string, word_cnt> word_map; //用来暂存词频的映射表 


            //对标题进行分词    
            std::vector<std::string> title_words;    
            ns_util::JiebaUtil::CutString(doc.title, &title_words); 

            //对标题进行词频统计       
            for(auto s : title_words)     
            {    
                boost::to_lower(s); // 将我们的分词进行统一转化成为小写的    
                // 一个关键词 对应 标题 中出现的次数
                word_map[s].title_cnt++;//如果存在就获取，不存在就新建    
            }    

            //对文档内容进行分词    
            std::vector<std::string> content_words;    
            ns_util::JiebaUtil::CutString(doc.content, &content_words);  

            //对文档内容进行词频统计       
            for(auto s : content_words)      
            {    
                boost::to_lower(s); // 将我们的分词进行统一转化成为小写的    
                word_map[s].content_cnt++;     // 一个关键词 对应 内容 中出现的次数
            }

            #define X 10    
            #define Y 1 

            //最终构建倒排  
            for(auto &word_pair : word_map)    
            {    
                InvertedElem item;    //定义一个倒排拉链节点，然后填写相应的字段
                item.doc_id = doc.doc_id; //倒排索引的id即文档id   
                item.word = word_pair.first;    // word_pair.first-> 关键字
                item.weight = X * word_pair.second.title_cnt + Y * word_pair.second.content_cnt;    //权重计算
                InvertedList& inverted_list = inverted_index[word_pair.first];    
                inverted_list.push_back(std::move(item));    // 将关键字 对应的 倒排拉链节点 保存到 对应的倒排拉链这个 数组中
            }

            return true;
        } 
    };

    // 单例模式
    Index* Index::instance = nullptr;
    std::mutex Index::mtx;
}
