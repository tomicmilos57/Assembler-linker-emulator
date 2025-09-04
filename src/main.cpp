#include "stdio.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdint>
#include <string>
#include <list>
#include <vector>

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
    printf("halt called\n");
}

void iret() {
    printf("iret called\n");
}

void softint() {
    printf("int (software interrupt) called\n");
}

void ret() {
    printf("ret called\n");
}

void call(char *op) {
    printf("call %s\n", op);
}

void jmp(char *op) {
    printf("jmp %s\n", op);
}

void push(char *r) {
    printf("push %s\n", r);
}

void pop(char *r) {
    printf("pop %s\n", r);
}

void notinst(char *r) {
    printf("not %s\n", r);
}

void xchg(char *rs, char *rd) {
    printf("xchg %s,%s\n", rs, rd);
}

void add(char *rs, char *rd) {
    printf("add %s,%s\n", rs, rd);
}

void sub(char *rs, char *rd) {
    printf("sub %s,%s\n", rs, rd);
}

void mul(char *rs, char *rd) {
    printf("mul %s,%s\n", rs, rd);
}

void divinst(char *rs, char *rd) {
    printf("div %s,%s\n", rs, rd);
}

void andinst(char *rs, char *rd) {
    printf("and %s,%s\n", rs, rd);
}

void orinst(char *rs, char *rd) {
    printf("or %s,%s\n", rs, rd);
}

void xorinst(char *rs, char *rd) {
    printf("xor %s,%s\n", rs, rd);
}

void shl(char *rs, char *rd) {
    printf("shl %s,%s\n", rs, rd);
}

void shr(char *rs, char *rd) {
    printf("shr %s,%s\n", rs, rd);
}

void ld(char *op, char *r) {
    printf("ld %s,%s\n", op, r);
}

void st(char *r, char *op) {
    printf("st %s,%s\n", r, op);
}

void csrrd(char *csr, char *r) {
    printf("csrrd %s,%s\n", csr, r);
}

void csrwr(char *r, char *csr) {
    printf("csrwr %s,%s\n", r, csr);
}

void beq(char *r1, char *r2, char *op) {
    printf("beq %s,%s,%s\n", r1, r2, op);
}

void bne(char *r1, char *r2, char *op) {
    printf("bne %s,%s,%s\n", r1, r2, op);
}

void bgt(char *r1, char *r2, char *op) {
    printf("bgt %s,%s,%s\n", r1, r2, op);
}


void test(){
  printf("Found test\n");
}

void add_global(char *sym)    { printf(".global %s\n", sym); }
void add_extern(char *sym)    { printf(".extern %s\n", sym); }
void set_section(char *name)  { printf(".section %s\n", name); }
void add_word(char *op)       { printf(".word %s\n", op); }
void skip_bytes(int n)        { printf(".skip %d\n", n); }
void add_ascii(char *s)       { printf(".ascii %s\n", s); }
void define_equ(char *sym, char *val) { printf(".equ %s, %s\n", sym, val); }
void equ(char *name, OperandNode *expr_list) { 
  printf(".equ %s", name);
  for (OperandNode* node = expr_list; node; node = node->next) {
    printf(", %s", node->val);
  }
  printf("\n");
}
void finish()                 { printf(".end\n"); }

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
  printf("Finnished parsing\n");

  return 0;
}
