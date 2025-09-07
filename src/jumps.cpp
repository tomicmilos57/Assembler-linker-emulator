#include "defs.hpp"

extern Sections sections;
extern SymbolTable symtable;
extern FillTable filltable;

extern void insert_instruction(uint32_t instruction, uint32_t regA, uint32_t regB, uint32_t regC, uint32_t disp);
extern int get_reg(char* chars);
extern int get_csr(char* chars);

void insert_symbol_jumps(char* sym, uint32_t instruction, uint32_t regA, uint32_t regB, uint32_t regC){

  std::string str = std::string(sym);
  bool found = symtable.map.contains(str);

  if (found){

    sections.getCurrentSection()->insert_relocation(sym, 0);
    insert_instruction(instruction, regA, regB, regC, 0);

  } else{

    symtable.createEntry(0, false, true, sym);
    sections.getCurrentSection()->insert_relocation(sym, 0);
    insert_instruction(instruction, regA, regB, regC, 0);

  }

}

void call_literal(int val) {
  debugf("call %d\n", val);

  insert_instruction(STORE_INC_A_C_D, SP, 0, PC, -4); //oduzmi sp i upisi na mem[sp] u istom taktu

  filltable.createLiteralEntry(val, LOAD_MEM_BC_D, PC, R0, R0, 0); //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
  insert_instruction(0, 0, 0, 0, 0);
}

void call_symbol(char *sym) {
  debugf("call %s\n", sym);

  insert_instruction(STORE_INC_A_C_D, SP, 0, PC, -4); //oduzmi sp i upisi na mem[sp] u istom taktu
  insert_symbol_jumps(sym, JMP_AB_D, R0, 0, 0);
}

void jmp_literal(int val) {
  debugf("jmp %d\n", val);

  filltable.createLiteralEntry(val, LOAD_MEM_BC_D, PC, R0, R0, 0); //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
  insert_instruction(0, 0, 0, 0, 0);
}

void jmp_symbol(char *sym) {
  debugf("jmp %s\n", sym);

  insert_symbol_jumps(sym, JMP_AB_D, R0, 0, 0);
}

void beq_literal(char *r1, char *r2, int val) {
  debugf("beq %s,%s,%d\n", r1, r2, val);

  int reg1 = get_reg(r1);
  int reg2 = get_reg(r2);
  filltable.createLiteralEntry(val, JEQ_MEM_AB_C_D, R0, reg1, reg2, 0); //if(gpr[B]==gpr[C]) pc<=mem32[gpr[A]+D];
  insert_instruction(0, 0, 0, 0, 0);
}

void beq_symbol(char *r1, char *r2, char *sym) {
  debugf("beq %s,%s,%s\n", r1, r2, sym);

  int reg1 = get_reg(r1);
  int reg2 = get_reg(r2);
  insert_symbol_jumps(sym, JEQ_AB_C_D, R0, reg1, reg2);
}

void bne_literal(char *r1, char *r2, int val) {
  debugf("bne %s,%s,%d\n", r1, r2, val);

  int reg1 = get_reg(r1);
  int reg2 = get_reg(r2);
  filltable.createLiteralEntry(val, JNE_MEM_AB_C_D, R0, reg1, reg2, 0); //if(gpr[B]==gpr[C]) pc<=mem32[gpr[A]+D];
  insert_instruction(0, 0, 0, 0, 0);
}

void bne_symbol(char *r1, char *r2, char *sym) {
  debugf("bne %s,%s,%s\n", r1, r2, sym);

  int reg1 = get_reg(r1);
  int reg2 = get_reg(r2);
  insert_symbol_jumps(sym, JNE_AB_C_D, R0, reg1, reg2);
}

void bgt_literal(char *r1, char *r2, int val) {
  debugf("bgt %s,%s,%d\n", r1, r2, val);

  int reg1 = get_reg(r1);
  int reg2 = get_reg(r2);
  filltable.createLiteralEntry(val, JGT_MEM_AB_C_D, R0, reg1, reg2, 0); //if(gpr[B]==gpr[C]) pc<=mem32[gpr[A]+D];
  insert_instruction(0, 0, 0, 0, 0);
}

void bgt_symbol(char *r1, char *r2, char *sym) {
  debugf("bgt %s,%s,%s\n", r1, r2, sym);

  int reg1 = get_reg(r1);
  int reg2 = get_reg(r2);
  insert_symbol_jumps(sym, JGT_AB_C_D, R0, reg1, reg2);
}
