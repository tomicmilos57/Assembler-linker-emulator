#include "defs.hpp"

void halt() {
    debugf("halt called\n");
}

void iret() {
    debugf("iret called\n");
}

void softint() {
    debugf("int (software interrupt) called\n");
}

void ret() {
    debugf("ret called\n");
}

void call(char *op) {
    debugf("call %s\n", op);
}

void jmp(char *op) {
    debugf("jmp %s\n", op);
}

void push(char *r) {
    debugf("push %s\n", r);
}

void pop(char *r) {
    debugf("pop %s\n", r);
}

void notinst(char *r) {
    debugf("not %s\n", r);
}

void xchg(char *rs, char *rd) {
    debugf("xchg %s,%s\n", rs, rd);
}

void add(char *rs, char *rd) {
    debugf("add %s,%s\n", rs, rd);

}

void sub(char *rs, char *rd) {
    debugf("sub %s,%s\n", rs, rd);
}

void mul(char *rs, char *rd) {
    debugf("mul %s,%s\n", rs, rd);
}

void divinst(char *rs, char *rd) {
    debugf("div %s,%s\n", rs, rd);
}

void andinst(char *rs, char *rd) {
    debugf("and %s,%s\n", rs, rd);
}

void orinst(char *rs, char *rd) {
    debugf("or %s,%s\n", rs, rd);
}

void xorinst(char *rs, char *rd) {
    debugf("xor %s,%s\n", rs, rd);
}

void shl(char *rs, char *rd) {
    debugf("shl %s,%s\n", rs, rd);
}

void shr(char *rs, char *rd) {
    debugf("shr %s,%s\n", rs, rd);
}

void ld(char *op, char *r) {
    debugf("ld %s,%s\n", op, r);
}

void st(char *r, char *op) {
    debugf("st %s,%s\n", r, op);
}

void csrrd(char *csr, char *r) {
    debugf("csrrd %s,%s\n", csr, r);
}

void csrwr(char *r, char *csr) {
    debugf("csrwr %s,%s\n", r, csr);
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
