#ifndef DEFS_HPP
#define DEFS_HPP

#include <string>
#include "classes.hpp"

#define LOG 0
#if LOG
#define debugf(format, ...) \
    do { \
        printf(format, ##__VA_ARGS__); \
    } while(0)
#else
#define debugf(format, ...) \
    do { \
    } while(0)
#endif

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

typedef struct OperandNode {
    char *val;
    struct OperandNode *next;
} OperandNode;

void add_global(char *sym);
void add_extern(char *sym);
void set_section(char *name);
void add_word(char *op);
void skip_bytes(int n);
void add_ascii(char *s);
void define_equ(char *sym, char *val);
void equ(char *name, OperandNode *expr_list);
void finish();

void halt();
void iret();
void softint();
void ret();

void push(char *r);
void pop(char *r);
void notinst(char *r);

void xchg(char *rs, char *rd);
void add(char *rs, char *rd);
void sub(char *rs, char *rd);
void mul(char *rs, char *rd);
void divinst(char *rs, char *rd);
void andinst(char *rs, char *rd);
void orinst(char *rs, char *rd);
void xorinst(char *rs, char *rd);
void shl(char *rs, char *rd);
void shr(char *rs, char *rd);
void csrrd(char *csr, char *r);
void csrwr(char *r, char *csr);

void call_literal(int val);
void call_symbol(char *sym);
void jmp_literal(int val);
void jmp_symbol(char *sym);
void beq_literal(char *r1, char *r2, int val);
void beq_symbol(char *r1, char *r2, char *sym);
void bne_literal(char *r1, char *r2, int val);
void bne_symbol(char *r1, char *r2, char *sym);
void bgt_literal(char *r1, char *r2, int val);
void bgt_symbol(char *r1, char *r2, char *sym);

void ld_literal(int val, char *r);
void ld_symbol(char *sym, char *r);
void ld_memory(char *mem, char *r);
void st_literal(char *r, int val);
void st_symbol(char *r, char *sym);
void st_memory(char *r, char *mem);

#endif
