#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <cstdint>

struct symbolTableEntry {
  int value;
  bool local;
  bool found;
  std::string symbol;
  std::string section_name = "";
};

class SymbolTable {
  public:
    void createEntry(int value, bool found, bool local, const std::string &symbol, const std::string &section) {
      auto *entry = new symbolTableEntry{value, local, found, symbol, section};
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
          << std::dec << 0 << " "
          << (entry->local ? "LOC " : "GLOB ")
          << entry->symbol;

        if (!entry->section_name.empty())
          out << " " << entry->section_name;

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
        int dummy;
        std::string bind;
        std::string symbol;
        std::string section;

        ls >> index_colon;
        if (index_colon.back() == ':')
          index_colon.pop_back();

        if (!(ls >> value_hex >> dummy >> bind >> symbol))
          continue;
        ls >> section;

        int value = 0;
        try {
          value = std::stoi(value_hex, nullptr, 16);
        } catch (...) {
          continue;
        }

        bool local = (bind == "LOC");
        bool found = true;

        table.createEntry(value, found, local, symbol, section);
      }
      return table;
    }

    std::vector<symbolTableEntry *> list;
    std::unordered_map<std::string, symbolTableEntry *> map;
};

typedef struct relocationEntry {
  uint32_t offset;
  std::string symbol;
  int addend;
} relocation;

class Section {
  public:
    Section(std::string name) : name(std::move(name)) {}

    uint8_t array[4 * 1024]{};
    uint32_t offset = 0;
    std::string name;
    std::vector<relocation *> list_of_relocations;

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

    Section* findOrCreateSection(const std::string &name) {
      for (auto *s : sections) {
        if (s->name == name) return s;
      }
      Section *s = new Section(name);
      sections.push_back(s);
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
            ls >> std::hex >> r->offset >> r->symbol >> r->addend;
            current->list_of_relocations.push_back(r);
          } else {
            std::istringstream ls(line);
            std::string byteStr;
            while (ls >> byteStr) {
              uint8_t b = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
              current->addByte(b);
            }
          }
        }
      }

      obj.symtab = SymbolTable::from_string(symtab_lines);
      return obj;
    }

    // Merge another Sections object into this one
    void merge(const Sections &other) {
      // merge symbols
      for (auto *entry : other.symtab.list) {
        if (symtab.map.find(entry->symbol) == symtab.map.end()) {
          symtab.createEntry(entry->value, entry->found, entry->local, entry->symbol, entry->section_name);
        }
      }

      // merge sections
      for (auto *sec : other.sections) {
        Section *dst = findOrCreateSection(sec->name);
        // copy bytes
        for (uint32_t i = 0; i < sec->offset; i++) {
          dst->addByte(sec->array[i]);
        }
        // copy relocations
        for (auto *r : sec->list_of_relocations) {
          relocation *nr = new relocation{r->offset, r->symbol, r->addend};
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
            << r->offset << " " << r->symbol << " " << std::dec << r->addend << "\n";
        }
      }
    }
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <obj_file>...\n";
    return 1;
  }

  Sections all;

  for (int i = 1; i < argc; i++) {
    std::ifstream infile(argv[i]);
    if (!infile) {
      std::cerr << "Error: cannot open file " << argv[i] << "\n";
      return 1;
    }

    std::ostringstream buffer;
    buffer << infile.rdbuf();
    std::string content = buffer.str();

    Sections obj = Sections::parse(content);
    all.merge(obj);
  }

  all.dump();

  return 0;
}
