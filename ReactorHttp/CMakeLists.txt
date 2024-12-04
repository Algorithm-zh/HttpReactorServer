cmake_minimum_required(VERSION 3.0)
project(Server.app)
file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
include_directories(
  ${PROJECT_SOURCE_DIR}/common
  ${PROJECT_SOURCE_DIR}/http
  ${PROJECT_SOURCE_DIR}/reactor
  ${PROJECT_SOURCE_DIR}/tcp
  ${PROJECT_SOURCE_DIR}/thread
)
# 添加子目录
add_subdirectory(common)
add_subdirectory(http)
add_subdirectory(reactor)
add_subdirectory(tcp)
add_subdirectory(thread)
# 连接静态库文件
link_libraries(tcp http reactor thread common)
# 指定可执行文件生成到对应的目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 添加并生成一个可执行程序
add_executable(Server.app ${SRC_LIST})
