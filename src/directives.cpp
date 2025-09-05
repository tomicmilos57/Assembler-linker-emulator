#include "defs.hpp"

extern Sections sections;
extern SymbolTable symtable;
extern FillTable filltable;


void add_global(char *sym) {
  debugf(".global %s\n", sym);

  symtable.createEntry(0, false, sym);
}

void add_extern(char *sym)    { 
  debugf(".extern %s\n", sym);

  symtable.createEntry(0, true, sym);
}

void set_section(char *name)  { 
  debugf(".section %s\n", name);

  Section* sec = new Section(std::string(name));
  sections.add_section(sec);

  symtable.createEntry(0, false, name);
}

void add_word(int n) {
  debugf(".word_int %d\n", n);

  sections.getCurrentSection()->insert_int(n);
}

void add_word(char *s){
  debugf(".word_sym %s\n", s);

  sections.getCurrentSection()->insert_relocation(s, 0);

  sections.getCurrentSection()->insert_int(0);
}

void skip_bytes(int n) {
  debugf(".skip %d\n", n);

  sections.getCurrentSection()->offset += n;
}

void add_ascii(char *s) {
  debugf(".ascii %s\n", s);

  std::string str = std::string(s);
  str = str.substr(1, str.size() - 2);
  for(char c : str) {
    sections.getCurrentSection()->insert_byte((uint8_t)c);
  }
}

void define_equ(char *sym, char *val) {
  debugf(".equ %s, %s\n", sym, val);
}

void equ(char *name, OperandNode *expr_list) { 
  debugf(".equ %s", name);
  for (OperandNode* node = expr_list; node; node = node->next) {
    debugf(", %s", node->val);
  }
  debugf("\n");
}
void finish() { debugf(".end\n"); }
