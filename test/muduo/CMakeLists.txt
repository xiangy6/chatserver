cmake_minimum_required(VERSION 2.0)
project(main)

#set(CMAKE_CXX_STANDARD 11)
#配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)

#配置头文件搜索路径
#include_directories()
#配置库文件搜索路径
#link_directories()

#设置生成的可执行文件存放目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设置需要编译的源文件列表
#set(SRC_LIST muduo_server.cpp)

#可执行文件server，由SRC_LIST变量所定义的源文件编译而来
add_executable(server muduo_server.cpp)

#表示server这个目标程序，需要链接muduo_net muduo_base pthread这三个库文件
target_link_libraries(server muduo_net muduo_base pthread)