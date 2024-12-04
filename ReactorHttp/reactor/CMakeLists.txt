cmake_minimum_required(VERSION 3.0)
project(reactor)

aux_source_directory(./ SRC)
# 把源文件制作成静态库
add_library(reactor STATIC ${SRC})

