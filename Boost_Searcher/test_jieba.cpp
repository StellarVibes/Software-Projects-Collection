#include <iostream>
#include <string>
#include <vector>
#include "cppjieba/Jieba.hpp"

int main()
{
    std::cout << "====================================" << std::endl;
    std::cout << "cppjieba Segmentation Test" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << std::endl;

    const char* const DICT_PATH = "./cppjieba/dict/jieba.dict.utf8";
    const char* const HMM_PATH = "./cppjieba/dict/hmm_model.utf8";
    const char* const USER_DICT_PATH = "./cppjieba/dict/user.dict.utf8";
    const char* const IDF_PATH = "./cppjieba/dict/idf.utf8";
    const char* const STOP_WORD_PATH = "./cppjieba/dict/stop_words.utf8";

    std::cout << "[INFO] Initializing cppjieba..." << std::endl;
    std::cout << "Dictionary path: " << DICT_PATH << std::endl;
    std::cout << std::endl;

    try
    {
        cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
        std::cout << "[SUCCESS] cppjieba initialized successfully!" << std::endl;
        std::cout << std::endl;

        std::vector<std::string> words;
        std::string s1 = "我来到北京清华大学";
        std::string s2 = "小明硕士毕业于中国科学院计算所";
        std::string s3 = "我爱北京天安门";

        std::cout << "====================================" << std::endl;
        std::cout << "Test 1: " << s1 << std::endl;
        std::cout << "====================================" << std::endl;
        jieba.CutForSearch(s1, words);
        std::cout << "Segmentation result: ";
        for (auto word : words)
        {
            std::cout << word << " / ";
        }
        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << "====================================" << std::endl;
        std::cout << "Test 2: " << s2 << std::endl;
        std::cout << "====================================" << std::endl;
        jieba.CutForSearch(s2, words);
        std::cout << "Segmentation result: ";
        for (auto word : words)
        {
            std::cout << word << " / ";
        }
        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << "====================================" << std::endl;
        std::cout << "Test 3: " << s3 << std::endl;
        std::cout << "====================================" << std::endl;
        jieba.CutForSearch(s3, words);
        std::cout << "Segmentation result: ";
        for (auto word : words)
        {
            std::cout << word << " / ";
        }
        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << "====================================" << std::endl;
        std::cout << "[SUCCESS] All tests passed!" << std::endl;
        std::cout << "====================================" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ERROR] cppjieba initialization or execution failed!" << std::endl;
        std::cerr << "Error message: " << e.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << "Please check:" << std::endl;
        std::cerr << "1. Dictionary files exist in cppjieba/dict/ directory" << std::endl;
        std::cerr << "2. Dictionary files are complete" << std::endl;
        std::cerr << "3. File encoding is UTF-8" << std::endl;
        return 1;
    }

    std::cout << std::endl;
    std::cout << "Press any key to exit...";
    std::cin.get();
    return 0;
}
