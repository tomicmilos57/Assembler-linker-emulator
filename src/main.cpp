#include "stdio.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs.hpp"

extern void yyparse();
extern FILE* yyin;

Sections sections;
SymbolTable symtable;
FillTable filltable;

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

  sections = *(new Sections());
  symtable = *(new SymbolTable());
  filltable = *(new FillTable());

  yyparse();
  printf("Finnished parsing\n");

  return 0;
}
