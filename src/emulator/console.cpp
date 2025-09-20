#include "console.hpp"
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

Console::Console(CPU& cpu) : cpu(cpu) {}

Console::~Console() {}

void Console::simulate_input(){
  setNonBlockingInput(true);

  char c;
  if (read(STDIN_FILENO, &c, 1) > 0) {
    send_char(c);
  }

  setNonBlockingInput(false);
}

void Console::send_char(char c){
  cpu.set_inter(c);
  //std::cout << "Sent" << std::endl;
}

void Console::setNonBlockingInput(bool enable) {
  static termios oldt;
  static bool isEnabled = false;

  if (enable && !isEnabled) {
    termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    isEnabled = true;
  } else if (!enable && isEnabled) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, 0);
    isEnabled = false;
  }
}

Timer::Timer(CPU& cpu) : cpu(cpu) {
  begin = std::chrono::steady_clock::now();
}

void Timer::simulate_timer(){
  end = std::chrono::steady_clock::now();

  if(cpu.get_timer_cfg() < std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()){
    //std::cout << "Timer tick " << cpu.get_timer_cfg() << std::endl;
    begin = std::chrono::steady_clock::now();
    cpu.set_timer_inter();
  }

}
