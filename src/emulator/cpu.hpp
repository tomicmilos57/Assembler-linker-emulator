#ifndef CPU_h
#define CPU_h
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <map>

class CPU {

public:
  CPU(const std::string &filename);
  ~CPU();

  bool execute();
  void info_registers();
  void info_csr_registers();
  void info_pc();
  uint32_t get_pc();
  uint32_t get_instruction_number();
  void info_ir();
  void info_instruction_number();
  void printMemory();

private:
enum instruction {
    // Halt
    i_halt,

    // Int
    i_int,

    // Iret
    i_iret,

    // Call
    i_call,
    i_call_ind,  // call indirektno

    // Jump
    i_jmp,
    i_jmp_ind,
    i_beq,
    i_bne,
    i_bgt,
    i_beq_ind,
    i_bne_ind,
    i_bgt_ind,

    // Xchg
    i_xchg,

    // Aritmetičke
    i_add,
    i_sub,
    i_mul,
    i_div,

    // Logičke
    i_not,
    i_and,
    i_or,
    i_xor,

    // Shift
    i_shl,
    i_shr,

    // Store
    i_st_direct,      // mem32[gpr[A]+gpr[B]+D] <= gpr[C]
    i_st_autoinc,     // gpr[A] <= gpr[A]+D; mem32[gpr[A]] <= gpr[C]
    i_st_indirect,    // mem32[mem32[gpr[A]+gpr[B]+D]] <= gpr[C]

    // Load
    i_ld_csr,         // gpr[A] <= csr[B]
    i_ld_imm,         // gpr[A] <= gpr[B] + D
    i_ld_mem,         // gpr[A] <= mem32[gpr[B]+gpr[C]+D]
    i_ld_autoinc,     // gpr[A] <= mem32[gpr[B]]; gpr[B] <= gpr[B] + D
    i_csrwr_gpr,      // csr[A] <= gpr[B]
    i_csrwr_imm,      // csr[A] <= csr[B] + D
    i_csrwr_mem,      // csr[A] <= mem32[gpr[B]+gpr[C]+D]
    i_csrwr_autoinc,  // csr[A] <= mem32[gpr[B]]; gpr[B] <= gpr[B] + D

    i_invalid_instruction
};

  uint32_t regfile[16] = {};
  uint32_t csrfile[3] = {};

  uint32_t &pc = regfile[15];
  uint32_t &sp = regfile[14];

  uint32_t &status = csrfile[0];
  uint32_t &handler = csrfile[1];
  uint32_t &cause = csrfile[2];

  uint32_t ir = 0;
  uint32_t instruction_number = 0;
 
  std::map<uint32_t, uint8_t> memory;

  std::ofstream ofile;

  instruction decode_instruction();
  void execute_instruction(instruction inst);
  uint32_t fetch_word(uint32_t address);
  void store_word(uint32_t address, uint32_t value);
  void push(uint32_t val);
  uint32_t pop();
};


#endif
