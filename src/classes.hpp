#include <cstdint>
#include <string>
#include <list>
#include <vector>

#include <sstream>
#include <iomanip>

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
  uint32_t size = 0;
  std::vector<uint32_t> list_of_literals;
  uint32_t offset = 0;
  std::string name;
  std::vector<relocationEntry*> list_of_relocations;

  std::string section_to_string() const;
  std::string literals_to_string() const;
  std::string relocations_to_string() const;

  void insert_int(int n);
  void insert_byte(uint8_t b);
  void insert_relocation(char* symbol, int addend);
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
  std::string sections_to_string() const;
};


class SymbolTable {
public:
  void createEntry(int value, bool local, char* symbol);

  std::vector<symbolTableEntry*> list;
  std::string to_string() const;
};


class FillTable {
public:
  std::vector<toFill*> list;
  std::string to_string() const;
};
