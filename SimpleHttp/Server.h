#pragma once
// 初始化用于监听的套接字
int initListenFd(unsigned short);
// 启动epoll
int epollRun(int lfd);
// 和客户端建立新链接
int acceptClient(int lfd, int epfd);
