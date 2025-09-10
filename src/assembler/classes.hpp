#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>

#include <sstream>
#include <iomanip>

typedef struct symbolTableEntry{

  int value;
  bool local;
  bool found;
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
  uint32_t final_size = 0;
  std::string name;
  std::vector<relocationEntry*> list_of_relocations;

  std::string section_to_string() const;
  std::string literals_to_string() const;
  std::string relocations_to_string() const;

  void insert_int(uint32_t n);
  void insert_byte(uint8_t b);
  void insert_relocation(char* symbol, int addend);
  void insert_relocation(char* symbol, int addend, uint32_t custom_offset);
};

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
  void finnishAssembly();
};

class SymbolTable {
public:
  void createEntry(int value, bool found, bool local, char* symbol);

  std::vector<symbolTableEntry*> list;
  std::unordered_map<std::string, symbolTableEntry*> map;
  std::string to_string() const;
};

typedef struct toFill{

  Section* section;
  uint32_t offset;
  uint32_t literalOffset;
  char* symchar;
  std::string symbol;

  uint32_t instruction;
  uint32_t regA;
  uint32_t regB;
  uint32_t regC;
  uint32_t disp;

  bool literal;
} toFill;

class FillTable {
public:
  void createSymbolEntry(char* symbol, uint32_t instruction, uint32_t regA, uint32_t regB, uint32_t regC, uint32_t disp);
  void createLiteralEntry(int val, uint32_t instruction, uint32_t regA, uint32_t regB, uint32_t regC, uint32_t disp);
  void finnishAssembly();

  std::vector<toFill*> list;
  std::string to_string() const;
};
