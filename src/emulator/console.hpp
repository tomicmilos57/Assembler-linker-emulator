#ifndef CONSOLE_H
#define CONSOLE_H
#include <cstdint>
#include "cpu.hpp"

class Console {
public:
  Console(CPU& cpu);
  ~Console();
  void simulate_input();

private:
  CPU& cpu;
  void setNonBlockingInput(bool enable);
  void send_char(char c);
};

#endif
