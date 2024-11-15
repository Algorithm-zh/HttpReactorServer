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
  return 0;
}
