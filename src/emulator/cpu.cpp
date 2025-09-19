#include "cpu.hpp"
#include <iostream>
#include <iomanip>

CPU::CPU(const std::string &filename) {
  std::ifstream infile(filename);
  std::string line;

  while (std::getline(infile, line)) {
    if (line.empty()) continue;

    std::stringstream ss(line);
    std::string addrStr;
    ss >> addrStr;

    if (addrStr.back() == ':')
      addrStr.pop_back();

    uint32_t baseAddr = std::stoul(addrStr, nullptr, 16);

    std::string byteStr;
    uint32_t offset = 0;
    while (ss >> byteStr) {
      uint8_t byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
      memory[baseAddr + offset] = byte;
      offset++;
    }
  }
}
CPU::~CPU(){
  ofile.close();
}

bool CPU::execute(){
  this->ir = fetch_word(this->pc);
  instruction_number++;

  CPU::instruction inst = decode_instruction();

  regfile[0] = 0;

  execute_instruction(inst);
  regfile[0] = 0;
 
  return true;
}

void CPU::info_registers(){

    for (int i = 0; i < 16; ++i) {
    ofile << "x" << std::setw(2) << std::setfill('0') << i << ": 0x"
      << std::hex << std::setw(8)<< regfile[i] << std::dec << std::endl;
  }
}

void CPU::info_csr_registers(){
  ofile << "CSR Registers:\n";

  auto print_csr = [](const std::string& name, uint32_t value, std::ofstream& ofile) {
    ofile << std::left << std::setw(12) << std::setfill(' ') << name << std::setfill('0') << std::right << ": 0x"
      << std::hex << std::setw(8) << value << std::dec << std::endl;
  };

  print_csr("status", csrfile[0], ofile);
  print_csr("handler", csrfile[1], ofile);
  print_csr("cause", csrfile[2], ofile);
}

uint32_t CPU::get_pc(){
  return regfile[15];
}
void CPU::info_pc(){
  ofile << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << regfile[15] << std::endl;
}
void CPU::info_ir(){
  ofile << "IR: 0x" << std::hex << std::setw(8) << std::setfill('0') << ir << std::endl;
}

uint32_t CPU::get_instruction_number(){
  return instruction_number;
}
void CPU::info_instruction_number(){
  ofile << std::dec << "instruction number: " << instruction_number << std::endl;
}

uint32_t CPU::fetch_word(uint32_t address) {
  return  (static_cast<uint32_t>(memory[address])) |
    (static_cast<uint32_t>(memory[address + 1]) << 8) |
    (static_cast<uint32_t>(memory[address + 2]) << 16) |
    (static_cast<uint32_t>(memory[address + 3]) << 24);
}

void CPU::execute_instruction(instruction inst){

}

CPU::instruction CPU::decode_instruction() {

  uint8_t OC = this->ir && 0xF0000000;
  uint8_t MOD = this->ir && 0x0F000000;

  uint8_t REGA = this->ir && 0x00F00000;
  uint8_t REGB = this->ir && 0x000F0000;
  uint8_t REGC = this->ir && 0x0000F000;

  uint16_t Disp = this->ir && 0x00000FFF;

  switch (OC) {
    case 0x0: return i_halt;
    case 0x1: return i_int;
    case 0x2: {
                switch (MOD) {
                  case 0x0: return i_call;
                  case 0x1: return i_call_ind;
                  default: return i_invalid_instruction;
                }
              }
    case 0x3: switch (MOD) {
                  case 0x0: return i_jmp;
                  case 0x1: return i_beq;
                  case 0x2: return i_bne;
                  case 0x3: return i_bgt;
                  case 0x8: return i_jmp_ind;
                  case 0x9: return i_beq_ind;
                  case 0xA: return i_bne_ind;
                  case 0xB: return i_bgt_ind;
                default: return i_invalid_instruction;
              }
    case 0x4: return i_xchg;
    case 0x5: switch (MOD) {
                  case 0x0: return i_add;
                  case 0x1: return i_sub;
                  case 0x2: return i_mul;
                  case 0x3: return i_div;
                default: return i_invalid_instruction;
              }
    case 0x6: switch (MOD) {
                  case 0x0: return i_not;
                  case 0x1: return i_and;
                  case 0x2: return i_or;
                  case 0x3: return i_xor;
                default: return i_invalid_instruction;
              }
    case 0x7: switch (MOD) {
                  case 0x0: return i_shl;
                  case 0x1: return i_shr;
                default: return i_invalid_instruction;
              }
    case 0x8: switch (MOD) {
                  case 0x0: return i_st_direct;
                  case 0x2: return i_st_indirect;
                  case 0x1: return i_st_autoinc;
                default: return i_invalid_instruction;
              }
    case 0x9: switch (MOD) {
                  case 0x0: return i_ld_csr;
                  case 0x1: return i_ld_imm;
                  case 0x2: return i_ld_mem;
                  case 0x3: return i_ld_autoinc;
                  case 0x4: return i_csrwr_gpr;
                  case 0x5: return i_csrwr_imm;
                  case 0x6: return i_csrwr_mem;
                  case 0x7: return i_csrwr_autoinc;
                default: return i_invalid_instruction;
              }
    default:
      return i_invalid_instruction;
  }
  return i_invalid_instruction;
}

void CPU::printMemory(){
  for (auto &[addr, val] : memory) {
    std::cout << std::hex << addr << ": " 
      << std::setw(2) << std::setfill('0') << (int)val << "\n";
  }
}
