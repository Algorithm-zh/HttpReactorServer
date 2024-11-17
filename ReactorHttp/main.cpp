#include "Log.h"
#include "TcpServer.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if (argc < 3) {
    std::cout << "./a.out port path\n" << std::endl;
    return -1;
  }
  unsigned short port = atoi(argv[1]);
  // 切换服务器工作路径
  chdir(argv[2]);
  // 启动服务器啊
  Debug("开始运行");

  TcpServer *server = new TcpServer(port, 4);
  server->run();
  return 0;
}
