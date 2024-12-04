cmake_minimum_required(VERSION 3.0)
project(tcp)

aux_source_directory(./ SRC)
# 把源文件制作成静态库
add_library(tcp STATIC ${SRC})

