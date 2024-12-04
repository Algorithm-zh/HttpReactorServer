cmake_minimum_required(VERSION 3.0)
project(thread)

aux_source_directory(./ SRC)
# 把源文件制作成静态库
add_library(thread STATIC ${SRC})
target_link_libraries(thread pthread)

