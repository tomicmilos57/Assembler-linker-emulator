#include "defs.hpp"

extern Sections sections;
extern SymbolTable symtable;
extern FillTable filltable;

void patch_instruction(Section* section, uint32_t offset,
    uint32_t instruction, uint32_t regA, uint32_t regB, uint32_t regC, uint32_t disp){

  uint32_t i_instruction = instruction << 24;
  uint32_t i_regA = regA << 20;
  uint32_t i_regB = regB << 16;
  uint32_t i_regC = regC << 12;
  uint32_t i_disp = disp & 0xFFF;


  debugf("patching: 0x%x\n", i_instruction | i_regA | i_regB | i_regC | i_disp);
  *(uint32_t*)&(section->array)[offset] = i_instruction | i_regA | i_regB | i_regC | i_disp;
}

void Sections::finnishAssembly(){
  for(Section* section : list){
    uint32_t pos = section->offset;

    for (uint32_t literal : section->list_of_literals) {
      section->array[pos++] = (literal >> 0) & 0xFF;
      section->array[pos++] = (literal >> 8) & 0xFF;
      section->array[pos++] = (literal >> 16) & 0xFF;
      section->array[pos++] = (literal >> 24) & 0xFF;
    }
  }

}

void FillTable::finnishAssembly(){
  for(toFill* entry : list){

    patch_instruction(entry->section, entry->offset,
        entry->instruction, entry->regA, entry->regB, entry->regC,
        entry->section->offset + entry->literalOffset*4);

    if (!entry->literal) {
      entry->section->insert_relocation(entry->symchar, 0,
          entry->section->offset + entry->literalOffset*4);
    }
  }
}

void SymbolTable::createEntry(int value, bool found, bool local, char* symbol){

  symbolTableEntry* entry = new symbolTableEntry;

  entry->value = value;
  entry->local = local;
  entry->found = found;
  entry->symbol = std::string(symbol);

  list.push_back(entry);
  map[entry->symbol] = entry;

}

std::string SymbolTable::to_string() const {
  std::ostringstream out;

  out << "#.symtab\n";
  out << "Num Value    Size Type  Bind Ndx Name\n";

  for (size_t i = 0; i < list.size(); ++i) {
    const auto &entry = list[i];

    out << std::dec << i << ": "
      << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << entry->value
      << " "
      << std::dec << 0
      << " ";

    out << "NOTYP ";

    out << (entry->local ? "LOC " : "GLOB ");

    out << entry->symbol << "\n";
  }

  return out.str();
}

void Section::insert_int(uint32_t n){
  *(uint32_t*)&array[offset] = n;
  offset += 4;
}

void Section::insert_byte(uint8_t b){
  array[offset] = b;
  offset++;
}

void Section::insert_relocation(char* symbol, int addend){

  relocationEntry* entry = new relocationEntry;

  entry->offset = this->offset;
  entry->symbol = std::string(symbol);
  entry->addend = addend;

  list_of_relocations.push_back(entry);

}

void Section::insert_relocation(char* symbol, int addend, uint32_t custom_offset){

  relocationEntry* entry = new relocationEntry;

  entry->offset = custom_offset;
  entry->symbol = std::string(symbol);
  entry->addend = addend;

  list_of_relocations.push_back(entry);

}

std::string Section::section_to_string() const {
  std::ostringstream out;

  out << "#." << name << "\n";

  for (uint32_t i = 0; i < offset; i++) {
    out << std::hex << std::setfill('0') << std::setw(2)
      << static_cast<int>(array[i]);

    if ((i % 16) != 15 && i + 1 < offset)
      out << " ";

    if ((i % 16) == 15)
      out << "\n";
  }

  if (offset % 16 != 0)
    out << "\n";

  return out.str();
}

std::string Section::literals_to_string() const {
  std::ostringstream out;

  out << "#." << name << ".literals\n";

  size_t byte_count = 0;

  for (size_t i = 0; i < list_of_literals.size(); i++) {
    uint32_t lit = list_of_literals[i];

    for (int b = 0; b < 4; b++) {
      uint8_t byte = static_cast<uint8_t>((lit >> (8 * b)) & 0xFF);
      out << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<int>(byte);

      byte_count++;

      if ((byte_count % 16) != 0 && !(i == list_of_literals.size()-1 && b == 3))
        out << " ";

      if (byte_count % 16 == 0)
        out << "\n";
    }
  }

  if (byte_count % 16 != 0)
    out << "\n";

  return out.str();
}


std::string Section::relocations_to_string() const {
  std::ostringstream out;

  out << "#.rela." << name << "\n";
  out << "Offset  Symbol  Addend\n";

  for (const auto &rel : list_of_relocations) {
    out << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << rel->offset << " ";
    out << rel->symbol << " ";
    out << std::dec << rel->addend << "\n";
  }

  return out.str();
}

std::string Sections::sections_to_string() const{
  std::ostringstream out;

  for (const auto &sec : list) {
    out << sec->section_to_string();
    out << sec->literals_to_string();
    out << sec->relocations_to_string();
  }

  return out.str();
}

void FillTable::createSymbolEntry(char* symbol, uint32_t instruction, uint32_t regA, uint32_t regB, uint32_t regC, uint32_t disp){
  toFill* entry = new toFill();
  debugf("ToFillSymbol inst: 0x%x\n", instruction | regA | regB | regC | disp);
  sections.getCurrentSection()->list_of_literals.push_back(0); //creating empty 4 bytes for symbol

  entry->section = sections.getCurrentSection();
  entry->offset = sections.getCurrentSection()->offset;

  entry->literal = false;
  entry->literalOffset = sections.getCurrentSection()->list_of_literals.size() - 1;

  std::string str = std::string(symbol);
  bool found = symtable.map.contains(str);

  if (!found){
    symtable.createEntry(0, false, true, symbol);
  }
  //Do relocations after parsing
  //sections.getCurrentSection()->insert_relocation(sym, 0);

  entry->symbol = std::string(symbol);
  entry->symchar = symbol;

  entry->instruction =  instruction;
  entry->regA = regA;
  entry->regB = regB;
  entry->regC = regC;
  entry->disp = disp;

  this->list.push_back(entry);
}

void FillTable::createLiteralEntry(int val, uint32_t instruction, uint32_t regA, uint32_t regB, uint32_t regC, uint32_t disp){
  toFill* entry = new toFill();
  debugf("ToFillLiteral inst: 0x%x\n", instruction | regA | regB | regC | disp);

  sections.getCurrentSection()->list_of_literals.push_back(val);

  entry->section = sections.getCurrentSection();
  entry->offset = sections.getCurrentSection()->offset;

  entry->literal = true;
  entry->literalOffset = sections.getCurrentSection()->list_of_literals.size() - 1;
  entry->symbol = std::to_string(val);

  entry->instruction =  instruction;
  entry->regA = regA;
  entry->regB = regB;
  entry->regC = regC;
  entry->disp = disp;

  this->list.push_back(entry);
}

std::string FillTable::to_string() const {
  std::ostringstream out;

  out << "#.filltable\n";
  out << "Num Section   Offset   Symbol\n";

  for (size_t i = 0; i < list.size(); i++) {
    const auto &entry = list[i];

    out << std::setw(3) << std::dec << i << " ";
    out << std::setw(8) << (entry->section ? entry->section->name : "NULL") << " ";
    out << "0x" << std::setw(6) << std::setfill('0') << std::hex << entry->offset << " ";
    out << entry->symbol << "\n";
  }

  return out.str();
}
