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

void call(char *op);
void jmp(char *op);
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
void ld(char *op, char *r);
void st(char *r, char *op);
void csrrd(char *csr, char *r);
void csrwr(char *r, char *csr);

void beq(char *r1, char *r2, char *op);
void bne(char *r1, char *r2, char *op);
void bgt(char *r1, char *r2, char *op);

#endif
