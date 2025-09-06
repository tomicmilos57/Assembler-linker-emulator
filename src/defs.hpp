#ifndef DEFS_HPP
#define DEFS_HPP

#include <string>
#include "classes.hpp"

#define LOG 1
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
