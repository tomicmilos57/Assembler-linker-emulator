#include "stdio.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "defs.hpp"

extern void yyparse();
extern FILE* yyin;

Sections sections;
SymbolTable symtable;
FillTable filltable;

void print_everything(){
  std::string string_symtable = symtable.to_string();
  std::string string_sections = sections.sections_to_string();
  std::string string_filltable = filltable.to_string();

  std::cout << "Symtable:\n"   << symtable.to_string()
    << "\nSections:\n" << sections.sections_to_string()
    << "\nFillTable:\n" << filltable.to_string()
    << std::endl;
}

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
  //print_everything();
  printf("Finnished parsing\n");

  return 0;
}
