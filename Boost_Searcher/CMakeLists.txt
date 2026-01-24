cmake_minimum_required(VERSION 3.10)
project(Boost_Searcher)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
    add_compile_options(/utf-8)
else()
    add_compile_options(-Wall -Wextra)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cppjieba/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cppjieba/deps/limonp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test/cpp-httplib-v0.7.15)

set(DICT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cppjieba/dict")
add_definitions(-DDICT_PATH="${DICT_PATH}")

find_package(Boost 1.65.0 REQUIRED COMPONENTS system filesystem)

find_package(jsoncpp CONFIG REQUIRED)

if(WIN32)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

add_executable(parser parser.cpp)
target_link_libraries(parser Boost::system Boost::filesystem)

add_executable(debug debug.cpp)
target_link_libraries(debug JsonCpp::JsonCpp)

add_executable(http_server http_server.cpp)
target_link_libraries(http_server JsonCpp::JsonCpp)
if(WIN32)
    target_link_libraries(http_server Threads::Threads)
else()
    target_link_libraries(http_server pthread)
endif()

add_executable(test_jieba test_jieba.cpp)
target_link_libraries(test_jieba)

message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
message(STATUS "Dictionary path: ${DICT_PATH}")
message(STATUS "JsonCpp found: ${jsoncpp_FOUND}")
