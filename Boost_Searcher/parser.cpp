#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>
#include "util.hpp"

// 首先我们肯定会读取文件，所以先将文件的路径名 罗列出来
// 将 数据源的路径 和 清理后干净文档的路径 定义好


const std::string src_path = "data/input";          // 数据源的路径
const std::string output = "data/raw_html/raw.txt"; // 清理后干净文档的路径

//DocInfo --- 文件信息结构体
typedef struct DocInfo
{
    std::string title;   //文档的标题
    std::string content; //文档的内容
    std::string url;     //该文档在官网当中的url
}DocInfo_t;

// 命名规则
// const & ---> 输入
// * ---> 输出
// & ---> 输入输出


// 由于C++ 和 STL 对文件系统的支持并不是特别好，我们想要完成一下操作，需要使用Boost库


//把每个html文件名带路径，保存到files_list中
bool EnumFile(const std::string &src_path, std::vector<std::string> *files_list);
 
//按照files_list读取每个文件的内容，并进行解析
bool ParseHtml(const std::vector<std::string> &files_list, std::vector<DocInfo_t> *results);
 
//把解析完毕的各个文件的内容写入到output
bool SaveHtml(const std::vector<DocInfo_t> &results, const std::string &output);



int main()
{
    std::vector<std::string> files_list; // 将所有的 html文件名保存在 files_list 中

    // 第一步：递归式的把每个html文件名带路径，保存到files_list中，方便后期进行一个一个的文件读取

    // 从 src_path 这个路径中提取 html文件，将提取出来的文件存放在 string 类型的 files_list 中
    if(!EnumFile(src_path, &files_list)) //EnumFile--枚举文件
    {
        std::cerr << "enum file name error! " << std::endl;
        return 1;
    }

    // 第二步：从 files_list 文件中读取每个.html的内容，并进行解析

     std::vector<DocInfo_t> results;
     // 从 file_list 中进行解析，将解析出来的内容存放在 DocInfo 类型的 results 中
    if(!ParseHtml(files_list, &results))//ParseHtml--解析html
    {
        std::cerr << "parse html error! " << std::endl;
        return 2;
    }


    // 第三部：把解析完毕的各个文件的内容写入到output，按照 \3 作为每个文档的分隔符

    // 将 results 解析好的内容，直接放入 output 路径下
    if(!SaveHtml(results, output))//SaveHtml--保存html
    {
        std::cerr << "save html error! " << std::endl;
        return 3;
    }

    return 0;
}

//把每个html文件名带路径，保存到files_list中
bool EnumFile(const std::string &src_path, std::vector<std::string> *files_list)
{
    // 简化作用域的书写
    namespace fs = boost::filesystem;
    fs::path root_path(src_path); // 定义一个path对象，枚举文件就从这个路径下开始
    // 判断路径是否存在
    if(!fs::exists(root_path))
    {
        std::cerr << src_path << " not exists" << std::endl;
        return false;
    }
    // 对文件进行递归遍历
    fs::recursive_directory_iterator end; // 定义了一个空的迭代器，用来进行判断递归结束  -- 相当于 NULL
    for(fs::recursive_directory_iterator iter(root_path); iter != end; iter++)
    {
        // 判断指定路径是不是常规文件，如果指定路径是目录或图片直接跳过
        if(!fs::is_regular_file(*iter))
        {
            continue;
        }
 
        // 如果满足了是普通文件，还需满足是.html结尾的
        // 如果不满足也是需要跳过的
        // ---通过iter这个迭代器（理解为指针）的一个path方法（提取出这个路径）
        // ---然后通过extension()函数获取到路径的后缀
        if(iter->path().extension() != ".html")
        {
            continue;
        }
 
        //std::cout << "debug: " << iter->path().string() << std::endl; // 测试代码
      
        // 走到这里一定是一个合法的路径，以.html结尾的普通网页文件
        files_list->push_back(iter->path().string()); // 将所有带路径的html保存在files_list中，方便后续进行文本分析
    }
    return true;
}


//解析title
static bool ParseTitle(const std::string& file,std::string* title)
{
    // 查找 <title> 位置
    std::size_t begin = file.find("<title>");
    if(begin == std::string::npos)
    {
        return false;
    }
    // 查找 </title> 位置
    std::size_t end = file.find("</title>");
    if(end == std::string::npos)
    {
        return false;
    }
    
    // 计算中间的距离，截取中间的内容
    begin += std::string("<title>").size();
    if(begin > end)
    {
        return false;
    }
    *title = file.substr(begin, end - begin);
    return true;
}

//去标签 -- 数据清洗
static bool ParseContent(const std::string& file,std::string* content)
{
     //去标签，基于一个简易的状态机
    enum status // 枚举两种状态
    {                                                                                                                                                                                 
        LABLE,   // 标签
        CONTENT  // 内容
    };
    enum status s = LABLE;  // 刚开始肯定会碰到 "<" 默认状态为 LABLE
    for(char c : file)
    {
        // 检测状态
        switch(s)
        {
            case LABLE:
                if(c == '>') s = CONTENT;
                break;
            case CONTENT:
                if(c == '<') s = LABLE;
                else 
                {
                    // 我们不想保留原始文件中的\n，因为我们想用\n作为html解析之后的文本的分隔符
                    if(c == '\n') c = ' ';
                    content->push_back(c);
                }
                break;
            default:
                break;
        }
    }
    return true;
}

//构建官网url :url_head + url_tail
static bool ParseUrl(const std::string& file_path,std::string* url)
{
    std::string url_head = "https://www.boost.org/doc/libs/1_85_0/doc/html";    
    std::string url_tail = file_path.substr(src_path.size());//将data/input截取掉    
    *url = url_head + url_tail;//拼接
    return true;
}

// for debug
static void ShowDoc(const DocInfo_t &doc)
{
    std::cout<<"title: "<<doc.title<<std::endl;
    std::cout<<"content: "<<doc.content<<std::endl;
    std::cout<<"url: "<<doc.url<<std::endl;
}

//按照files_list读取每个文件的内容，并进行解析
bool ParseHtml(const std::vector<std::string> &files_list, std::vector<DocInfo_t> *results)
{
    // 首先在解析文件之前，肯定需要 遍历 读取文件
    for(const std::string &file : files_list)
    {
        // 1.读取文件，Read() --- 将文件的全部内容全部读出，放到 result 中
        std::string result;
        if(!ns_util::FileUtil::ReadFile(file, &result))
        {
            continue;
        }
        // 2.解析指定的文件，提取title
        DocInfo_t doc;
        if(!ParseTitle(result, &doc.title))
        {
            continue;
        }
        // 3.解析指定的文件，提取content
        if(!ParseContent(result, &doc.content))
        {
            continue;
        }
        // 4.解析指定的文件路径，构建url
        if(!ParseUrl(file, &doc.url))
        {
            continue;        
        }

        // 到这里，一定是完成了解析任务，当前文档的相关结果都保存在了doc里面
        results->push_back(std::move(doc)); // 本质会发生拷贝，效率肯能会比较低，这里我们使用move后的左值变成了右值，去调用push_back的右值引用版本

        // for debug  -- 在测试的时候，将上面的代码改写为 results->push_back(doc);
        //ShowDoc(doc);
        //break;    
    }
    return true;
}


bool SaveHtml(const std::vector<DocInfo_t> &results, const std::string &output)
{
    #define SEP '\3'//分割符---区分标题、内容和网址
    
    // 打开文件，在里面进行写入
    // 按照二进制的方式进行写入 -- 你写的是什么文档就保存什么
    std::ofstream out(output, std::ios::out | std::ios::binary);
    if(!out.is_open())
    {
        std::cerr << "open " << output << " failed!" << std::endl;
        return false;
    }
 
    // 到这里就可以进行文件内容的写入了
    for(auto &item : results)
    {
        std::string out_string;
        out_string = item.title;//标题
        out_string += SEP;//分割符
        out_string += item.content;//内容
        out_string += SEP;//分割符
        out_string += item.url;//网址
        out_string += '\n';//换行，表示区分每一个文件

        // 将字符串内容写入文件中
        out.write(out_string.c_str(), out_string.size());
    }
 
    out.close();
    return true;
}