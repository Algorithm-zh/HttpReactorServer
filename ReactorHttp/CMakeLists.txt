cmake_minimum_required(VERSION 3.0)
project(Server.app)
file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# 指定源文件或者动态库对应的头文件路径
include_directories(${PROJECT_SOURCE_DIR})
# 添加并生成一个可执行程序
add_executable(Server.app ${SRC_LIST})
# 指定要链接的动态库
target_link_libraries(Server.app pthread)
