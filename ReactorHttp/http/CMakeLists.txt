cmake_minimum_required(VERSION 3.0)
project(http)

aux_source_directory(./ SRC)
# 把源文件制作成静态库
add_library(http STATIC ${SRC})

