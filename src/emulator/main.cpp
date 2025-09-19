#include "cpu.hpp"
#include "console.hpp"
#include <iostream>

int main(){
  std::cout << "Emu Start" << std::endl;
  CPU cpu("/home/milos/Projects/SS/resenje/build/binary.o");

  //cpu.info_registers();
  int count = 0;
  Console console(cpu);
  while (true) {
      console.simulate_input();
      count = 0;
    if(!cpu.execute()) break;

    count++;
  }
  cpu.info_registers();
  return 0;
}
