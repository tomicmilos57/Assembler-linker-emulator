#include "cpu.hpp"
#include "console.hpp"
#include <iostream>

int main(){
  std::cout << "Emu Start" << std::endl;
  CPU cpu("/home/milos/Projects/SS/resenje/build/binary.o");

  //cpu.info_registers();
  int count = 0;
  Console console(cpu);
  Timer timer(cpu);
  while (true) {
    console.simulate_input();
    timer.simulate_timer();

    if(!cpu.execute()) break;

    count++;
  }
  cpu.info_registers();
  return 0;
}
