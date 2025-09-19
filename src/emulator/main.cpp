#include "cpu.hpp"
#include <iostream>

int main(){
  std::cout << "Emu Start" << std::endl;
  CPU cpu("/home/milos/Projects/SS/resenje/build/binary.o");

  //cpu.info_registers();
  while (true) {
    if(!cpu.execute()) break;
  }
  cpu.info_registers();
  return 0;
}
