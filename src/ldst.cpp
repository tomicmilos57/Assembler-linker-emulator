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
  filltable.createLiteralEntry(val, LOAD_MEM_BC_D, reg, 0, 0, 0); //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
  insert_instruction(0, 0, 0, 0, 0);
}

void ld_abs_symbol(char *sym, char *r) {
  debugf("ld_abs_symbol: ld $%s,%s\n", sym, r);
}

void ld_dir_literal(int val, char *r) {
  debugf("ld_dir_literal: ld %d,%s\n", val, r);
}

void ld_dir_symbol(char *sym, char *r) {
  debugf("ld_dir_symbol: ld %s,%s\n", sym, r);
}

void ld_reg(char *reg, char *r) {
  debugf("ld_reg: ld %s,%s\n", reg, r);
}

void ld_mem_reg(char *reg, char *r) {
  debugf("ld_mem_reg: ld [%s],%s\n", reg, r);
}

void ld_mem_reg_off_literal(char *reg, int val, char *r) {
  debugf("ld_mem_reg_off_literal: ld [%s+%d],%s\n", reg, val, r);
}

void ld_mem_reg_off_symbol(char *reg, char *sym, char *r) {
  debugf("ld_mem_reg_off_symbol: ld [%s+%s],%s\n", reg, sym, r);
}

void st_abs_literal(char *r, int val) {
  debugf("st_abs_literal: st %s,$%d\n", r, val);
}

void st_abs_symbol(char *r, char *sym) {
  debugf("st_abs_symbol: st %s,$%s\n", r, sym);
}

void st_dir_literal(char *r, int val) {
  debugf("st_dir_literal: st %s,%d\n", r, val);
}

void st_dir_symbol(char *r, char *sym) {
  debugf("st_dir_symbol: st %s,%s\n", r, sym);
}

void st_reg(char *r, char *reg) {
  debugf("st_reg: st %s,%s\n", r, reg);
}

void st_mem_reg(char *r, char *reg) {
  debugf("st_mem_reg: st %s,[%s]\n", r, reg);
}

void st_mem_reg_off_literal(char *r, char *reg, int val) {
  debugf("st_mem_reg_off_literal: st %s,[%s+%d]\n", r, reg, val);
}

void st_mem_reg_off_symbol(char *r, char *reg, char *sym) {
  debugf("st_mem_reg_off_symbol: st %s,[%s+%s]\n", r, reg, sym);
}
