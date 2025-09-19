#include "cpu.hpp"


int main(){
  CPU cpu("/home/milos/Projects/SS/resenje/build/binary.o");
  //cpu.printMemory();

  while (true) {
    if(!cpu.execute()) break;
  }
  return 0;
}
