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

class Section {

public:
  Section(std::string name) : name(name) {}

  uint8_t array[4*1024];
  std::vector<uint32_t> list_of_literals;
  uint32_t offset = 0;
  std::string name;
  std::vector<relocationEntry> list_of_relocations;

};

typedef struct toFill{

  Section* section;
  uint8_t* sectionEntry;
  std::string symbol;

} toFill;

class Sections {
public:
  std::list<Section*> list;

  void add_section(Section* section){
    list.push_back(section);
  }
  Section* getCurrentSection(){
    return list.back();
  }
};


class SymbolTable {
public:
  std::vector<symbolTableEntry> list;
};


class FillTable {
public:
  std::vector<toFill> list;
};

