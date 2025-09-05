%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
extern "C" int yylex(void);

// Instruction handlers
void halt();
void softint();
void iret();
void call(char *op);
void ret();
void jmp(char *op);
void beq(char *r1, char *r2, char *op);
void bne(char *r1, char *r2, char *op);
void bgt(char *r1, char *r2, char *op);
void push(char *r);
void pop(char *r);
void xchg(char *rs, char *rd);
void add(char *rs, char *rd);
void sub(char *rs, char *rd);
void mul(char *rs, char *rd);
void divinst(char *rs, char *rd);
void notinst(char *r);
void andinst(char *rs, char *rd);
void orinst(char *rs, char *rd);
void xorinst(char *rs, char *rd);
void shl(char *rs, char *rd);
void shr(char *rs, char *rd);
void ld(char *op, char *r);
void st(char *r, char *op);
void csrrd(char *csr, char *r);
void csrwr(char *r, char *csr);

// Directive handlers
void add_global(char *sym);
void add_extern(char *sym);
void set_section(char *name);
void add_word(int n);
void add_word(char *s);
void skip_bytes(int n);
void add_ascii(char *s);
void define_equ(char *sym, char *value);
void equ(char *name, struct OperandNode *expr_list);
void finish();

typedef struct OperandNode {
    char *val;
    struct OperandNode *next;
} OperandNode;

%}

%union {
    char *s;
    int num;
    struct OperandNode *list;
}

%token <s> REGISTER SYMBOL STRING
%token <num> NUMBER
%type <list> operand_list operand_list_tail expr term
%type <s> operand memory_operand

%token HALT INT IRET CALL RET JMP BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR SHL SHR LD ST CSRRD CSRWR
%token GLOBAL EXTERN SECTION WORD SKIP ASCII EQU END LABEL

%%

program:
      /* empty */
    | program line
    ;

line:
      instruction
    | directive
    | label
    ;

instruction:
      HALT                                { halt(); }
    | INT                                 { softint(); }
    | IRET                                { iret(); }
    | CALL operand                        { call($2); }
    | RET                                 { ret(); }
    | JMP operand                         { jmp($2); }
    | BEQ REGISTER ',' REGISTER ',' operand { beq($2, $4, $6); }
    | BNE REGISTER ',' REGISTER ',' operand { bne($2, $4, $6); }
    | BGT REGISTER ',' REGISTER ',' operand { bgt($2, $4, $6); }
    | PUSH REGISTER                       { push($2); }
    | POP REGISTER                        { pop($2); }
    | XCHG REGISTER ',' REGISTER          { xchg($2, $4); }
    | ADD REGISTER ',' REGISTER           { add($2, $4); }
    | SUB REGISTER ',' REGISTER           { sub($2, $4); }
    | MUL REGISTER ',' REGISTER           { mul($2, $4); }
    | DIV REGISTER ',' REGISTER           { divinst($2, $4); }
    | NOT REGISTER                        { notinst($2); }
    | AND REGISTER ',' REGISTER           { andinst($2, $4); }
    | OR REGISTER ',' REGISTER            { orinst($2, $4); }
    | XOR REGISTER ',' REGISTER           { xorinst($2, $4); }
    | SHL REGISTER ',' REGISTER           { shl($2, $4); }
    | SHR REGISTER ',' REGISTER           { shr($2, $4); }
    | LD operand ',' REGISTER             { ld($2, $4); }
    | ST REGISTER ',' operand             { st($2, $4); }
    | CSRRD REGISTER ',' REGISTER         { csrrd($2, $4); }
    | CSRWR REGISTER ',' REGISTER         { csrwr($2, $4); }
    ;

directive:
      GLOBAL operand_list {
          for (OperandNode *p = $2; p; p = p->next) {
              add_global(p->val);
          }
      }
    | EXTERN operand_list {
          for (OperandNode *p = $2; p; p = p->next) {
              add_extern(p->val);
          }
      }
    | SECTION SYMBOL {
          set_section($2);
      }
    | WORD word_list {
          /* handled in word_list rules */
      }
    | SKIP NUMBER {
          skip_bytes($2);
      }
    | ASCII STRING {
          add_ascii($2);
      }
    | EQU SYMBOL ',' expr {
          equ($2, $4);
      }
    | END {
          finish();
      }
    ;

word_list:
      NUMBER { add_word($1); } word_list_rest
    | SYMBOL { add_word($1); } word_list_rest
    | STRING { add_word($1); } word_list_rest
    ;

word_list_rest:
      ',' NUMBER { add_word($2); } word_list_rest
    | ',' SYMBOL { add_word($2); } word_list_rest
    | ',' STRING { add_word($2); } word_list_rest
    |
    ;

label:
     SYMBOL ':' {
         free($1);
     }
   ;

operand:
      NUMBER {
          char buf[32];
          snprintf(buf, sizeof(buf), "%d", $1);
          $$ = strdup(buf);
      }
    | SYMBOL {
          $$ = $1;
      }
    | REGISTER {
          $$ = $1;
      }
    | memory_operand {
          $$ = $1; 
      }
    ;

operand_list:
      operand operand_list_tail {
          OperandNode *node = (OperandNode*)malloc(sizeof(OperandNode));
          node->val = $1;
          node->next = $2;
          $$ = node;
      }
    ;

operand_list_tail:
      ',' operand operand_list_tail {
          OperandNode *node = (OperandNode*)malloc(sizeof(OperandNode));
          node->val = $2;
          node->next = $3;
          $$ = node;
      }
    | /* empty */ {
          $$ = NULL;
      }
    ;

expr:
      term { $$ = $1; }
    | expr '+' term {
          OperandNode *t = $3;
          char *buf = (char*)malloc(strlen(t->val) + 2);
          sprintf(buf, "+%s", t->val);
          free(t->val);
          t->val = buf;

          OperandNode *p = $1;
          while (p->next) p = p->next;
          p->next = t;
          $$ = $1;
      }
    | expr '-' term {
          OperandNode *t = $3;
          char *buf = (char*)malloc(strlen(t->val) + 2);
          sprintf(buf, "-%s", t->val);
          free(t->val);
          t->val = buf;

          OperandNode *p = $1;
          while (p->next) p = p->next;
          p->next = t;
          $$ = $1;
      }
    ;

term:
      NUMBER {
          OperandNode *node = (OperandNode*)malloc(sizeof(OperandNode));
          char buf[32];
          snprintf(buf, sizeof(buf), "%d", $1);
          node->val = strdup(buf);
          node->next = NULL;
          $$ = node;
      }
    | SYMBOL {
          OperandNode *node = (OperandNode*)malloc(sizeof(OperandNode));
          node->val = $1;
          node->next = NULL;
          $$ = node;
      }
    ;

memory_operand:
      '[' REGISTER '+' NUMBER ']' {
          char buf[64];
          snprintf(buf, sizeof(buf), "[%s+%d]", $2, $4);
          $$ = strdup(buf);
      }
    | '[' REGISTER ']' {
          char buf[32];
          snprintf(buf, sizeof(buf), "[%s]", $2);
          $$ = strdup(buf);
      }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
    exit(255);
}
