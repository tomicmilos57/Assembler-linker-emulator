#include "defs.hpp"

#define SP 14
#define PC 15

#define R0 0
#define R1 1
#define R2 2
#define R3 3
#define R4 4
#define R5 5
#define R6 6
#define R7 7
#define R8 8
#define R9 9
#define R10 10
#define R11 11
#define R12 12
#define R13 13

#define STATUS 0
#define HANDLER 1
#define CAUSE 2

#define HALT 0x00  // OC=0x0, MOD=0x0
#define INT  0x10  // OC=0x1, MOD=0x0
#define CALL_AB_D   0x20  // OC=0x2, MOD=0x0  -> pc <= gpr[A] + gpr[B] + D
#define CALL_MEM_AB_D 0x21 // OC=0x2, MOD=0x1 -> pc <= mem32[gpr[A]+gpr[B]+D]
#define JMP_AB_D      0x30 // OC=0x3, MOD=0x0  -> pc <= gpr[A] + D
#define JEQ_AB_C_D    0x31 // OC=0x3, MOD=0x1  -> if(gpr[B]==gpr[C]) pc <= gpr[A]+D
#define JNE_AB_C_D    0x32 // OC=0x3, MOD=0x2  -> if(gpr[B]!=gpr[C]) pc <= gpr[A]+D
#define JGT_AB_C_D    0x33 // OC=0x3, MOD=0x3  -> if(gpr[B] signed > gpr[C]) pc <= gpr[A]+D
#define JMP_MEM_AB_D  0x38 // OC=0x3, MOD=0x8  -> pc <= mem32[gpr[A]+D]
#define JEQ_MEM_AB_C_D 0x39 // OC=0x3, MOD=0x9 -> if(gpr[B]==gpr[C]) pc<=mem32[gpr[A]+D]
#define JNE_MEM_AB_C_D 0x3A // OC=0x3, MOD=0xA -> if(gpr[B]!=gpr[C]) pc<=mem32[gpr[A]+D]
#define JGT_MEM_AB_C_D 0x3B // OC=0x3, MOD=0xB -> if(gpr[B] signed> gpr[C]) pc<=mem32[gpr[A]+D]
#define XCHG_BC 0x40 // OC=0x4, MOD=0x0
#define ADD 0x50  // OC=0x5, MOD=0x0
#define SUB 0x51  // OC=0x5, MOD=0x1
#define MUL 0x52  // OC=0x5, MOD=0x2
#define DIV 0x53  // OC=0x5, MOD=0x3
#define NOT 0x60  // OC=0x6, MOD=0x0
#define AND 0x61  // OC=0x6, MOD=0x1
#define OR  0x62  // OC=0x6, MOD=0x2
#define XOR 0x63  // OC=0x6, MOD=0x3
#define SHL 0x70  // OC=0x7, MOD=0x0
#define SHR 0x71  // OC=0x7, MOD=0x1
#define STORE_AB_C_D      0x80 // OC=0x8, MOD=0x0 -> mem32[gpr[A]+gpr[B]+D] <= gpr[C]
#define STORE_MEM_AB_C_D  0x82 // OC=0x8, MOD=0x2 -> mem32[mem32[gpr[A]+gpr[B]+D]] <= gpr[C]
#define STORE_INC_A_C_D   0x81 // OC=0x8, MOD=0x1 -> gpr[A]+=D; mem32[gpr[A]]<=gpr[C]
#define LOAD_CSR_B        0x90 // OC=0x9, MOD=0x0 -> gpr[A] <= csr[B]
#define LOAD_GPR_B_D      0x91 // OC=0x9, MOD=0x1 -> gpr[A] <= gpr[B]+D
#define LOAD_MEM_BC_D     0x92 // OC=0x9, MOD=0x2 -> gpr[A] <= mem32[gpr[B]+gpr[C]+D]
#define LOAD_MEM_B_INC_D  0x93 // OC=0x9, MOD=0x3 -> gpr[A] <= mem32[gpr[B]]; gpr[B]+=D
#define LOAD_CSR_A_B      0x94 // OC=0x9, MOD=0x4 -> csr[A] <= gpr[B]
#define LOAD_CSR_A_B_OR_D 0x95 // OC=0x9, MOD=0x5 -> csr[A] <= csr[B] | D
#define LOAD_CSR_MEM_BC_D 0x96 // OC=0x9, MOD=0x6 -> csr[A] <= mem32[gpr[B]+gpr[C]+D]
#define LOAD_CSR_MEM_B_INC 0x97 // OC=0x9, MOD=0x7 -> csr[A] <= mem32[gpr[B]]; gpr[B]+=D

extern Sections sections;
extern SymbolTable symtable;
extern FillTable filltable;

void insert_instruction(uint32_t instruction, uint32_t regA, uint32_t regB, uint32_t regC, uint32_t disp){

  uint32_t i_instruction = instruction << 24;
  uint32_t i_regA = regA << 20;
  uint32_t i_regB = regB << 16;
  uint32_t i_regC = regC << 12;
  uint32_t i_disp = disp & 0xFFF;


  debugf("inst: 0x%x\n", i_instruction | i_regA | i_regB | i_regC | i_disp);
  sections.getCurrentSection()->insert_int(i_instruction | i_regA | i_regB | i_regC | i_disp);
}

int get_reg(char* chars){
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

void beq(char *r1, char *r2, char *op) {
  debugf("beq %s,%s,%s\n", r1, r2, op);
}

void bne(char *r1, char *r2, char *op) {
  debugf("bne %s,%s,%s\n", r1, r2, op);
}

void bgt(char *r1, char *r2, char *op) {
  debugf("bgt %s,%s,%s\n", r1, r2, op);
}

void ld(char *op, char *r) {
  debugf("ld %s,%s\n", op, r);
}

void st(char *r, char *op) {
  debugf("st %s,%s\n", r, op);
}

void call(char *op) {
  debugf("call %s\n", op);
}

void jmp(char *op) {
  debugf("jmp %s\n", op);
}
