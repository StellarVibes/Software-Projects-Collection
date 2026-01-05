#include <iostream>
#include <string>
#include <vector>
#include <jsoncpp/json/json.h>

//Value Reader(反序列化) Writer(序列化)
int main()
{
    Json::Value root;
    Json::Value item1;
    item1["key1"]="value1";
    item1["key2"]="value2";

    Json::Value item2;
    item2["key1"]="value3";
    item2["key2"]="value4";

    root.append(item1);
    root.append(item2);

    // 两种序列化方式
    //Json::StyledWriter writer;  // 序列化方式1
    Json::FastWriter writer;  // 序列化方式2
    std::string s = writer.write(root);
    std::cout<<s<<std::endl;
    return 0;
}
