#include "stdio.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdint>
#include <string>
#include <list>
#include <vector>

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

typedef struct symbolTableEntry{

  int value;
  bool local;
  std::string symbol;

} symbolTableEntry;

typedef struct relocationEntry{

  uint32_t offset;
  std::string symbol;
  int addend;

} relocation;

typedef struct Section {

  uint8_t array[4*1024];
  std::vector<uint32_t> list_of_literals;
  uint32_t offset = 0;
  std::string name;
  std::vector<relocationEntry> list_of_relocations;

} section;

typedef struct toFill{

  Section* section;
  uint8_t* sectionEntry;
  std::string symbol;

} toFill;

std::list<Section> Sections;

std::vector<symbolTableEntry> SymbolTable;

std::vector<toFill> fillTable;



extern void yyparse();
extern FILE* yyin;
typedef struct OperandNode {
    char *val;
    struct OperandNode *next;
} OperandNode;

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


void test(){
  debugf("Found test\n");
}

void add_global(char *sym)    { debugf(".global %s\n", sym); }
void add_extern(char *sym)    { debugf(".extern %s\n", sym); }
void set_section(char *name)  { debugf(".section %s\n", name); }
void add_word(char *op)       { debugf(".word %s\n", op); }
void skip_bytes(int n)        { debugf(".skip %d\n", n); }
void add_ascii(char *s)       { debugf(".ascii %s\n", s); }
void define_equ(char *sym, char *val) { debugf(".equ %s, %s\n", sym, val); }
void equ(char *name, OperandNode *expr_list) { 
  debugf(".equ %s", name);
  for (OperandNode* node = expr_list; node; node = node->next) {
    debugf(", %s", node->val);
  }
  debugf("\n");
}
void finish()                 { debugf(".end\n"); }

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  yyin = fopen(argv[1], "r");
  if (!yyin) {
    perror("fopen");
    return 1;
  }
  yyparse();
  debugf("Finnished parsing\n");

  return 0;
}
