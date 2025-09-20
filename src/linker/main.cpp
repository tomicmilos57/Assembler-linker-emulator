#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <cstdint>

struct symbolTableEntry {
  uint32_t value;
  bool local;
  bool found;
  std::string symbol;
  std::string section_name = "";
  bool equ;
};

class SymbolTable {
  public:
    std::vector<symbolTableEntry *> list;
    std::unordered_map<std::string, symbolTableEntry *> map;

    void createEntry(uint32_t value, bool found, bool local, const std::string &symbol, const std::string &section, bool equ) {
      auto *entry = new symbolTableEntry{value, local, found, symbol, section, equ};
      list.push_back(entry);
      map[symbol] = entry;
    }

    std::string to_string() const {
      std::ostringstream out;
      out << "#.symtab\n";

      for (size_t i = 0; i < list.size(); ++i) {
        const auto &entry = list[i];

        out << std::dec << i << ": "
          << std::setw(8) << std::setfill('0')
          << std::hex << std::uppercase << entry->value
          << " "
          << std::dec << entry->found << " "
          << (entry->local ? "LOC " : "GLOB ")
          << entry->symbol;

        if (!entry->section_name.empty())
          out << " " << entry->section_name;

        out << " " << entry->equ;
        out << "\n";
      }

      return out.str();
    }

    static SymbolTable from_string(const std::vector<std::string> &lines) {
      SymbolTable table;
      for (auto &line : lines) {
        if (line.empty() || line[0] == '#')
          continue;

        std::istringstream ls(line);

        std::string index_colon;
        std::string value_hex;
        bool found;
        std::string bind;
        std::string symbol;
        std::string section;
        bool equ;

        ls >> index_colon;
        if (index_colon.back() == ':')
          index_colon.pop_back();

        if (!(ls >> value_hex >> found >> bind >> symbol))
          continue;
        ls >> section;
        ls >> equ;

        uint32_t value = 0;
        try {
          value = std::stoul(value_hex, nullptr, 16);
        } catch (...) {
          std::cout << "Exception" << std::endl;
          continue;
        }

        bool local = (bind == "LOC");

        table.createEntry(value, found, local, symbol, section, equ);
      }
      return table;
    }
};

typedef struct relocationEntry {
  uint32_t offset;
  std::string symbol;
  int addend;
  std::string section_name;
} relocation;

class Section {
  public:
    Section(std::string name) : name(std::move(name)) {}

    uint8_t array[4 * 1024]{};
    uint32_t offset = 0;
    std::string name;
    std::vector<relocation *> list_of_relocations;

    void insert_int(uint32_t offset, uint32_t n){
      *(uint32_t*)&array[offset] = n;
    }

    void addByte(uint8_t b) {
      if (offset < sizeof(array)) {
        array[offset++] = b;
      }
    }
};

class Sections {
  public:
    SymbolTable symtab;
    std::vector<Section *> sections;
    std::unordered_map<std::string, Section *> map;

    Section* findOrCreateSection(const std::string &name) {
      for (auto *s : sections) {
        if (s->name == name) return s;
      }
      Section *s = new Section(name);
      sections.push_back(s);
      map[s->name] = s;
      return s;
    }

    static Sections parse(const std::string &input) {
      Sections obj;
      std::istringstream iss(input);
      std::string line;

      Section *current = nullptr;
      bool in_symtab = false;
      std::vector<std::string> symtab_lines;

      while (std::getline(iss, line)) {
        if (line.empty())
          continue;

        if (line.rfind("#.", 0) == 0) {
          current = nullptr;

          if (line == "#.symtab") {
            in_symtab = true;
            continue;
          } else {
            in_symtab = false;
            std::string secname = line.substr(2);

            if (secname.rfind("rela.", 0) == 0) {
              secname = secname.substr(5);
            }

            current = obj.findOrCreateSection(secname);
            continue;
          }
        }

        if (in_symtab) {
          symtab_lines.push_back(line);
        } else if (current) {
          if (line.find_first_not_of("0123456789ABCDEFabcdef \t") != std::string::npos) {
            std::istringstream ls(line);
            relocation *r = new relocation{};
            ls >> std::hex >> r->offset >> r->symbol >> r->addend >> r->section_name;
            current->list_of_relocations.push_back(r);
          } else {
            std::istringstream ls(line);
            std::string byteStr;
            while (ls >> byteStr) {
              uint8_t b = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
              current->addByte(b);
            }
          }
        }
      }

      obj.symtab = SymbolTable::from_string(symtab_lines);
      return obj;
    }

    void merge(const Sections &other) {

      std::unordered_map<std::string, symbolTableEntry *> copy(symtab.map);
      for (auto *entry : other.symtab.list) {
        if (entry->found) {

          if (copy.contains(entry->section_name)) {
            if (entry->section_name != entry->symbol){
              if (!entry->equ) {
                symtab.createEntry(entry->value + this->map[entry->section_name]->offset, entry->found, entry->local, entry->symbol, entry->section_name, entry->equ);
              }
              else{
                symtab.createEntry(entry->value, entry->found, entry->local, entry->symbol, entry->section_name, entry->equ);
              }
            }
          }

          else{
            symtab.createEntry(entry->value, entry->found, entry->local, entry->symbol, entry->section_name, entry->equ);
          }

        }
      }

      for (auto *sec : other.sections) {
        Section *dst = findOrCreateSection(sec->name);
        uint32_t section_offset = dst->offset;

        for (uint32_t i = 0; i < sec->offset; i++) {
          dst->addByte(sec->array[i]);
        }

        for (auto *r : sec->list_of_relocations) {
          relocation *nr = new relocation{r->offset + section_offset, r->symbol, r->addend, r->section_name};
          dst->list_of_relocations.push_back(nr);
        }

      }

    }

    void dump() const {
      std::cout << symtab.to_string() << "\n";
      for (auto *sec : sections) {
        std::cout << "#." << sec->name << "\n";
        for (uint32_t i = 0; i < sec->offset; i++) {
          std::cout << std::setw(2) << std::setfill('0') << std::hex << std::uppercase
            << (int)sec->array[i] << " ";
          if ((i + 1) % 16 == 0)
            std::cout << "\n";
        }
        if (sec->offset % 16 != 0)
          std::cout << "\n";

        std::cout << "#.rela." << sec->name << "\n";
        for (auto *r : sec->list_of_relocations) {
          std::cout << std::setw(8) << std::setfill('0') << std::hex << std::uppercase
            << r->offset << " " << r->symbol << " " << std::dec << r->addend << " " << r->section_name << "\n";
        }
      }
    }

    void place_section(std::string section_name, int place){
      if(symtab.map.contains(section_name)){
        symtab.map[section_name]->value = place;
      }
      else
        std::cout << "Section: " << section_name << " doesnt exist" << std::endl;
    }

    void place_all_sections(){
      uint32_t next = 0;

      for(auto& entry : symtab.list){

        if (entry->symbol == entry->section_name && entry->value == 0) { //this means it is a section not label
          entry->value = next;
          next = entry->value + map[entry->symbol]->offset;
        }
      }

    }

    void resolve_relocations(){
      for (auto& section : sections){
        for (auto& entry : section->list_of_relocations){
          if (this->symtab.map.contains(entry->symbol) && this->symtab.map.contains(this->symtab.map[entry->symbol]->section_name)) {

            if (!this->symtab.map[entry->symbol]->equ) {
              section->insert_int(entry->offset, this->symtab.map[this->symtab.map[entry->symbol]->section_name]->value
                  + this->symtab.map[entry->symbol]->value + entry->addend);
            }
            else{ //equ
              section->insert_int(entry->offset, this->symtab.map[entry->symbol]->value + entry->addend);
            }

          }
          else{
            if(!this->symtab.map.contains(entry->symbol)){
              std::cout << "1. This Doesnt exist: " << entry->symbol << std::endl; 
            }
            if(!this->symtab.map.contains(this->symtab.map[entry->symbol]->section_name))
                std::cout << "2. This Doesnt exist: " << this->symtab.map[entry->symbol]->section_name << std::endl; 
          }

        }
      }
    }

    std::string binary() const {
      std::ostringstream out;

      for (auto* sec : sections) {
        if (sec->offset == 0) continue;

        //out << "#." << sec->name << "\n";

        uint32_t address = 0;
        if (symtab.map.count(sec->name)) {
          address = symtab.map.at(sec->name)->value;
        }

        for (uint32_t i = 0; i < sec->offset; i++) {
          if (i % 8 == 0) {
            out << std::setw(8) << std::setfill('0') << std::hex << std::uppercase
              << (address + i) << ": ";
          }

          out << std::setw(2) << std::setfill('0') << std::hex << std::uppercase
            << static_cast<int>(sec->array[i]) << " ";

          if ((i + 1) % 8 == 0) {
            out << "\n";
          }
        }

        if (sec->offset % 8 != 0) {
          out << "\n";
        }

        out << "\n";
      }

      return out.str();
    }
};


#include <getopt.h>

struct PlaceOption {
  std::string section;
  uint32_t address;
};

int main(int argc, char *argv[]) {
  bool hex = false;
  bool relocatable = false;
  std::string output_file = "./build/binary.o";
  std::vector<PlaceOption> places;

  Sections all;

  const struct option long_options[] = {
    {"place", required_argument, nullptr, 'p'},
    {"hex",   no_argument,       nullptr, 'h'},
    {"relocatable", no_argument, nullptr, 'r'},
    {"output", required_argument, nullptr, 'o'},
    {0, 0, 0, 0}
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "o:p:hr", long_options, nullptr)) != -1) {
    switch (opt) {
      case 'p': {
        std::string value(optarg);
        size_t at = value.find('@');
        if (at != std::string::npos) {
          PlaceOption p;
          p.section = value.substr(0, at);
          std::string addr_str = value.substr(at + 1);
          try {
            p.address = static_cast<uint32_t>(std::stoul(addr_str, nullptr, 0));
          } catch (const std::exception &e) {
            std::cerr << "Invalid address: " << addr_str << "\n";
            return 1;
          }
          places.push_back(p);
        }
        break;
      }
      case 'h':
        hex = true;
        break;
      case 'r':
        relocatable = true;
        break;
      case 'o':
        output_file = optarg;
        break;
      default:
        std::cerr << "usage: " << argv[0]
                  << " [--hex] [--relocatable] [--place=sec@addr] [-o outfile] <obj_file>...\n";
        return 1;
    }
  }

  for (int i = optind; i < argc; i++) {
    std::ifstream infile(argv[i]);
    if (!infile) {
      std::cerr << "error: cannot open file " << argv[i] << "\n";
      return 1;
    }
    std::ostringstream buffer;
    buffer << infile.rdbuf();
    std::string content = buffer.str();

    Sections obj = Sections::parse(content);
    all.merge(obj);
  }

  for (auto &p : places) {
    all.place_section(p.section, p.address);
  }

  all.place_all_sections();

  all.resolve_relocations();

  if (relocatable) {
    all.dump();
  }

  std::string binary = all.binary();
  if (hex) {
    std::ofstream hex;
    hex.open(std::string(output_file));
    hex << binary;
    hex.close();
  }

  return 0;
}
