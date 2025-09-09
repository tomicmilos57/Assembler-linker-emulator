#include "defs.hpp"
#include <cstring>

extern Sections sections;
extern SymbolTable symtable;
extern FillTable filltable;

void insert_instruction(uint32_t instruction, uint32_t regA, uint32_t regB, uint32_t regC, uint32_t disp){

  uint32_t i_instruction = (instruction & 0xFF) << 24;
  uint32_t i_regA = (regA & 0xF) << 20;
  uint32_t i_regB = (regB & 0xF) << 16;
  uint32_t i_regC = (regC & 0xF) << 12;
  uint32_t i_disp = disp & 0xFFF;


  debugf("inst: 0x%x\n", i_instruction | i_regA | i_regB | i_regC | i_disp);
  sections.getCurrentSection()->insert_int(i_instruction | i_regA | i_regB | i_regC | i_disp);
}

int get_reg(char* chars){
  if (!strcmp(chars, "sp")) {
    return 14;
  }
  if (!strcmp(chars, "pc")) {
    return 15;
  }
  if (chars == NULL || (chars[0] != 'r' && chars[0] != 'R')) {
    return -1;
  }
  int reg = atoi(chars + 1);
  if (reg < 0 || reg > 15) {
    return -1;
  }

  return reg;
}

int get_csr(char* chars){
  std::string str = std::string(chars);

  if (str == "status") return STATUS;
  else if (str == "handler") return HANDLER;
  else if (str == "cause") return CAUSE;

  return -1;
}

void halt() {
  debugf("halt called\n");

  insert_instruction(HALT,0,0,0,0);
}

void softint() {
  debugf("int (software interrupt) called\n");

  insert_instruction(INT,0,0,0,0);
}

void iret() {
  debugf("iret called\n");

  uint32_t instruction = 0x96000000;
  uint32_t regA = STATUS << 20;
  uint32_t regB = SP << 16; //sp
  uint32_t regC = R0 << 12; //r0
  uint32_t disp = 0 & 0xFFF;


  //STATUS
  insert_instruction(LOAD_CSR_MEM_BC_D, STATUS, SP, 0, 0); //upisi u sstatus mem[sp]
  insert_instruction(LOAD_GPR_B_D, SP, SP, 0, 4); //inc sp

  //PC
  insert_instruction(LOAD_MEM_BC_D, PC, SP, 0, 0); //upisi u pc mem[sp]
  insert_instruction(LOAD_GPR_B_D, SP, SP, 0, -4); //dec sp
}

void ret() {
  debugf("ret called\n");
 
  insert_instruction(LOAD_MEM_BC_D, PC, SP, 0, 0); //upisi u pc mem[sp]
  insert_instruction(LOAD_GPR_B_D, SP, SP, 0, -4); //dec sp
}

void push(char *r) {
  debugf("push %s\n", r);

  int reg = get_reg(r);
  insert_instruction(STORE_INC_A_C_D, SP, 0, reg, -4); //oduzmi sp i upisi na mem[sp] u istom taktu
}

void pop(char *r) {
  debugf("pop %s\n", r);

  int reg = get_reg(r);
  insert_instruction(LOAD_MEM_B_INC_D, reg, SP, 0, 4); //upisi reg na mem[sp] i sp+=4 u istom taktu
}

void notinst(char *r) {
  debugf("not %s\n", r);

  int reg = get_reg(r);
  insert_instruction(NOT, reg, reg, 0, 0); //not
}

void xchg(char *rs, char *rd) {
  debugf("xchg %s,%s\n", rs, rd);

  int regrd = get_reg(rd);
  int regrs = get_reg(rs);
  insert_instruction(XCHG_BC, 0, regrd, regrs, 0);
}

void add(char *rs, char *rd) {
  debugf("add %s,%s\n", rs, rd);

  int regrd = get_reg(rd);
  int regrs = get_reg(rs);
  insert_instruction(ADD, regrd, regrd, regrs, 0);
}

void sub(char *rs, char *rd) {
  debugf("sub %s,%s\n", rs, rd);

  int regrd = get_reg(rd);
  int regrs = get_reg(rs);
  insert_instruction(SUB, regrd, regrd, regrs, 0);
}

void mul(char *rs, char *rd) {
  debugf("mul %s,%s\n", rs, rd);

  int regrd = get_reg(rd);
  int regrs = get_reg(rs);
  insert_instruction(MUL, regrd, regrd, regrs, 0);
}

void divinst(char *rs, char *rd) {
  debugf("div %s,%s\n", rs, rd);

  int regrd = get_reg(rd);
  int regrs = get_reg(rs);
  insert_instruction(DIV, regrd, regrd, regrs, 0);
}

void andinst(char *rs, char *rd) {
  debugf("and %s,%s\n", rs, rd);

  int regrd = get_reg(rd);
  int regrs = get_reg(rs);
  insert_instruction(AND, regrd, regrd, regrs, 0);
}

void orinst(char *rs, char *rd) {
  debugf("or %s,%s\n", rs, rd);

  int regrd = get_reg(rd);
  int regrs = get_reg(rs);
  insert_instruction(OR, regrd, regrd, regrs, 0);
}

void xorinst(char *rs, char *rd) {
  debugf("xor %s,%s\n", rs, rd);

  int regrd = get_reg(rd);
  int regrs = get_reg(rs);
  insert_instruction(XOR, regrd, regrd, regrs, 0);
}

void shl(char *rs, char *rd) {
  debugf("shl %s,%s\n", rs, rd);

  int regrd = get_reg(rd);
  int regrs = get_reg(rs);
  insert_instruction(SHL, regrd, regrd, regrs, 0);
}

void shr(char *rs, char *rd) {
  debugf("shr %s,%s\n", rs, rd);

  int regrd = get_reg(rd);
  int regrs = get_reg(rs);
  insert_instruction(SHR, regrd, regrd, regrs, 0);
}

void csrrd(char *csr, char *r) {
  debugf("csrrd %s,%s\n", csr, r);

  int reg = get_reg(r);
  int regcsr = get_csr(csr);
  insert_instruction(LOAD_CSR_B, reg, regcsr, 0, 0);
}

void csrwr(char *r, char *csr) {
  debugf("csrwr %s,%s\n", r, csr);

  int reg = get_reg(r);
  int regcsr = get_csr(csr);
  insert_instruction(LOAD_CSR_A_B, regcsr, reg, 0, 0);
}
