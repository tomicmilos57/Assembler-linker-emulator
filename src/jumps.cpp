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

    symbolTableEntry* entry = symtable.map.at(str);

    if (entry->found){
      insert_instruction(instruction, regA, regB, regC, entry->value);
    } else{
      filltable.createEntry(sym, instruction, regA, regB, regC, entry->value);
      insert_instruction(0, 0, 0, 0, 0);
    }

  } else{
    symtable.createEntry(0, false, true, sym);
    filltable.createEntry(sym, instruction, regA, regB, regC, 0);
    insert_instruction(0, 0, 0, 0, 0);
  }

}

void call_literal(int val) {
  debugf("call %d\n", val);
}

void call_symbol(char *sym) {
  debugf("call %s\n", sym);
}

void jmp_literal(int val) {
  debugf("jmp %d\n", val);
}

void jmp_symbol(char *sym) {
  debugf("jmp %s\n", sym);

  insert_symbol_jumps(sym, JMP_AB_D, R0, 0, 0);
}

void beq_literal(char *r1, char *r2, int val) {
  debugf("beq %s,%s,%d\n", r1, r2, val);
}

void beq_symbol(char *r1, char *r2, char *sym) {
  debugf("beq %s,%s,%s\n", r1, r2, sym);

  int reg1 = get_reg(r1);
  int reg2 = get_reg(r2);
  insert_symbol_jumps(sym, JEQ_AB_C_D, R0, reg1, reg2);
}

void bne_literal(char *r1, char *r2, int val) {
  debugf("bne %s,%s,%d\n", r1, r2, val);
}

void bne_symbol(char *r1, char *r2, char *sym) {
  debugf("bne %s,%s,%s\n", r1, r2, sym);


  int reg1 = get_reg(r1);
  int reg2 = get_reg(r2);
  insert_symbol_jumps(sym, JNE_AB_C_D, R0, reg1, reg2);
}

void bgt_literal(char *r1, char *r2, int val) {
  debugf("bgt %s,%s,%d\n", r1, r2, val);
}

void bgt_symbol(char *r1, char *r2, char *sym) {
  debugf("bgt %s,%s,%s\n", r1, r2, sym);


  int reg1 = get_reg(r1);
  int reg2 = get_reg(r2);
  insert_symbol_jumps(sym, JGT_AB_C_D, R0, reg1, reg2);
}

void ld_literal(int val, char *r) {
  debugf("ld %d,%s\n", val, r);
}

void ld_symbol(char *sym, char *r) {
  debugf("ld %s,%s\n", sym, r);
}

void ld_memory(char *mem, char *r) {
  debugf("ld %s,%s\n", mem, r);
}

void st_literal(char *r, int val) {
  debugf("st %s,%d\n", r, val);
}

void st_symbol(char *r, char *sym) {
  debugf("st %s,%s\n", r, sym);
}

void st_memory(char *r, char *mem) {
  debugf("st %s,%s\n", r, mem);
}
