#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>

// ========== Symbol Table ==========

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
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ls(line);

        std::string index_colon;
        std::string value_hex;
        int dummy;
        std::string bind;     // GLOB or LOC
        std::string symbol;
        std::string section;

        ls >> index_colon;
        if (index_colon.back() == ':')
          index_colon.pop_back();

        if (!(ls >> value_hex >> dummy >> bind >> symbol))
          continue;
        ls >> section; // optional

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

    std::vector<symbolTableEntry*> list;
    std::unordered_map<std::string, symbolTableEntry*> map;
};

// ========== Section / ObjectFile ==========

struct Section {
  std::string name;
  std::vector<std::string> lines;   // raw text lines
};

struct ObjectFile {
  SymbolTable symtab;
  std::vector<Section> sections;

  static ObjectFile parse(const std::string &input) {
    ObjectFile obj;
    std::istringstream iss(input);
    std::string line;

    Section current;
    bool in_symtab = false;
    std::vector<std::string> symtab_lines;

    while (std::getline(iss, line)) {
      if (line.empty()) continue;

      if (line.rfind("#.", 0) == 0) {
        // starting new section
        if (!current.name.empty()) {
          obj.sections.push_back(current);
          current = Section{};
        }

        if (line == "#.symtab") {
          in_symtab = true;
          continue;
        } else {
          in_symtab = false;
          current.name = line.substr(2); // strip "#."
          continue;
        }
      }

      if (in_symtab) {
        symtab_lines.push_back(line);
      } else if (!current.name.empty()) {
        current.lines.push_back(line);
      }
    }

    if (!current.name.empty())
      obj.sections.push_back(current);

    obj.symtab = SymbolTable::from_string(symtab_lines);
    return obj;
  }

  void dump() const {
    std::cout << symtab.to_string() << "\n";
    for (auto &sec : sections) {
      std::cout << "#." << sec.name << "\n";
      for (auto &l : sec.lines)
        std::cout << l << "\n";
    }
  }
};

// ========== MAIN ==========

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <obj_file>\n";
    return 1;
  }

  std::ifstream infile(argv[1]);
  if (!infile) {
    std::cerr << "Error: cannot open file " << argv[1] << "\n";
    return 1;
  }

  std::ostringstream buffer;
  buffer << infile.rdbuf();
  std::string content = buffer.str();

  ObjectFile obj = ObjectFile::parse(content);

  // print everything back
  obj.dump();

  return 0;
}
