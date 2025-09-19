#include "defs.hpp"

extern Sections sections;
extern SymbolTable symtable;
extern FillTable filltable;

extern void insert_instruction(uint32_t instruction, uint32_t regA, uint32_t regB, uint32_t regC, uint32_t disp);
extern int get_reg(char* chars);
extern int get_csr(char* chars);

void ld_abs_literal(int val, char *r) {
  debugf("ld_abs_literal: ld $%d,%s\n", val, r);

  int reg = get_reg(r);
  //gpr[reg]<=mem32[D];
  filltable.createLiteralEntry(val, LOAD_MEM_BC_D, reg, PC, 0, 0); //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
  insert_instruction(0, 0, 0, 0, 0);
}

void ld_abs_symbol(char *sym, char *r) {
  debugf("ld_abs_symbol: ld $%s,%s\n", sym, r);

  int reg = get_reg(r);
  filltable.createSymbolEntry(sym, LOAD_MEM_BC_D, reg, PC, 0, 0); //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
  insert_instruction(LOAD_MEM_BC_D, reg, 0, 0, 0);
}

void ld_dir_literal(int val, char *r) {
  debugf("ld_dir_literal: ld %d,%s\n", val, r);

  int reg = get_reg(r);
  ld_abs_literal(val, r); //Loads literal into register
  //Rx <= mem[Rx];
  insert_instruction(LOAD_MEM_BC_D, reg, reg, 0, 0); //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
}

void ld_dir_symbol(char *sym, char *r) {
  debugf("ld_dir_symbol: ld %s,%s\n", sym, r);

  int reg = get_reg(r);
  ld_abs_symbol(sym, r); //Loads symbol into register
  //Rx <= mem[Rx];
  insert_instruction(LOAD_MEM_BC_D, reg, reg, 0, 0); //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
}

void ld_reg(char *reg, char *r) {
  debugf("ld_reg: ld %s,%s\n", reg, r);

  int regrs = get_reg(reg);
  int regrd = get_reg(r);
  insert_instruction(LOAD_GPR_B_D, regrd, regrs, 0, 0); //gpr[A]<=gpr[B]+D;
}

void ld_mem_reg(char *reg, char *r) {
  debugf("ld_mem_reg: ld [%s],%s\n", reg, r);

  int regrs = get_reg(reg);
  int regrd = get_reg(r);
  insert_instruction(LOAD_MEM_BC_D, regrd, regrs, 0, 0); //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
}

void ld_mem_reg_off_literal(char *reg, int val, char *r) {
  debugf("ld_mem_reg_off_literal: ld [%s+%d],%s\n", reg, val, r);
  if (val < -2048 || val > 2047){
    printf("Value for ld_mem_reg_off_literal greater than 12 bits\n");
    exit(12);
  }

  int regrs = get_reg(reg);
  int regrd = get_reg(r);
  insert_instruction(LOAD_MEM_BC_D, regrd, regrs, 0, val); //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
}

void ld_mem_reg_off_symbol(char *reg, char *sym, char *r) {
  debugf("ld_mem_reg_off_symbol: ld [%s+%s],%s\n", reg, sym, r);

  int regrs = get_reg(reg);
  int regrd = get_reg(r);
  filltable.createSymbolEntry(sym, LOAD_MEM_BC_D, regrd, regrs, 0, 0); //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
  insert_instruction(0, 0, 0, 0, 0);
}

void st_abs_literal(char *r, int val) {
  debugf("st_abs_literal: st %s,$%d\n", r, val);

  printf("st_abs_literal: Not allowed\n");
  exit(2);
}

void st_abs_symbol(char *r, char *sym) {
  debugf("st_abs_symbol: st %s,$%s\n", r, sym);

  printf("st_abs_symbol: Not allowed\n");
  exit(2);
}

void st_dir_literal(char *r, int val) {
  debugf("st_dir_literal: st %s,%d\n", r, val);

  int reg = get_reg(r);
  filltable.createLiteralEntry(val, STORE_MEM_AB_C_D, 0, PC, reg, 0); //mem32[mem32[gpr[A]+gpr[B]+D]]<=gpr[C];
  insert_instruction(0, 0, 0, 0, 0);
}

void st_dir_symbol(char *r, char *sym) {
  debugf("st_dir_symbol: st %s,%s\n", r, sym);

  int reg = get_reg(r);
  filltable.createSymbolEntry(sym, STORE_MEM_AB_C_D, 0, PC, reg, 0); //mem32[mem32[gpr[A]+gpr[B]+D]]<=gpr[C];
  insert_instruction(0, 0, 0, 0, 0);
}

void st_reg(char *r, char *reg) {
  debugf("st_reg: st %s,%s\n", r, reg);

  int operand = get_reg(reg);
  int gpr = get_reg(r);
  insert_instruction(LOAD_GPR_B_D, operand, gpr, 0, 0); //gpr[A]<=gpr[B]+D;
}

void st_mem_reg(char *r, char *reg) {
  debugf("st_mem_reg: st %s,[%s]\n", r, reg);

  int operand = get_reg(reg);
  int gpr = get_reg(r);
  insert_instruction(0x80, operand, 0, gpr, 0); //mem32[mem32[gpr[A]+gpr[B]+D]]<=gpr[C];
}

void st_mem_reg_off_literal(char *r, char *reg, int val) {
  debugf("st_mem_reg_off_literal: st %s,[%s+%d]\n", r, reg, val);

  int regrd = get_reg(reg);
  int regrs = get_reg(r);
  insert_instruction(STORE_AB_C_D, regrd, 0, regrs, val); //mem32[gpr[A]+gpr[B]+D]<=gpr[C];
}

void st_mem_reg_off_symbol(char *r, char *reg, char *sym) {
  debugf("st_mem_reg_off_symbol: st %s,[%s+%s]\n", r, reg, sym);

  int regrd = get_reg(reg);
  int regrs = get_reg(r);
  filltable.createSymbolEntry(sym, STORE_AB_C_D, regrd, 0, regrs, 0); //mem32[gpr[A]+gpr[B]+D]<=gpr[C];
  insert_instruction(0, 0, 0, 0, 0);
}
