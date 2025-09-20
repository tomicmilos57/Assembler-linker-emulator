#include "stdio.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "defs.hpp"

extern void yyparse();
extern FILE* yyin;

Sections sections;
SymbolTable symtable;
FillTable filltable;

void print_everything(std::ostream& out){
  out << symtable.to_string();
  out << sections.sections_to_string() << std::endl;
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
  debugf("Finnished parsing\n");

  sections.finnishAssembly();
  filltable.finnishAssembly();
  symtable.finnishAssembly();
  print_everything(std::cout);

  std::ofstream assembly;

  std::string inputFile = argv[1];

  if (inputFile.size() > 2 && inputFile.substr(inputFile.size() - 2) == ".s") {
    inputFile = inputFile.substr(0, inputFile.size() - 2);
  }

  assembly.open(std::string(inputFile + ".o"));
  print_everything(assembly);
  assembly.close();

  return 0;
}
