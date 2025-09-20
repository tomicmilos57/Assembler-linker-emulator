#ifndef CONSOLE_H
#define CONSOLE_H
#include <cstdint>
#include <chrono>
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

class Timer {
  public:
    Timer(CPU& cpu);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    void simulate_timer();

  private:
    CPU& cpu;
};

#endif
