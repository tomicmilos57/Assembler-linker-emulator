#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>

typedef struct symbolTableEntry {
    int value;
    bool local;
    bool found;
    std::string symbol;
    std::string section_name = "";
} symbolTableEntry;

class SymbolTable {
public:
    void createEntry(int value, bool found, bool local, const std::string &symbol, const std::string &section) {
        auto *entry = new symbolTableEntry{value, local, found, symbol, section};
        list.push_back(entry);
        map[symbol] = entry;
    }

    std::string to_string() const;

    static SymbolTable from_string(const std::string &input);

    std::vector<symbolTableEntry*> list;
    std::unordered_map<std::string, symbolTableEntry*> map;
};

std::string SymbolTable::to_string() const {
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

SymbolTable SymbolTable::from_string(const std::string &input) {
    SymbolTable table;
    std::istringstream iss(input);
    std::string line;

    bool in_symtab = false;

    while (std::getline(iss, line)) {
        if (line.empty()) continue;

        // section marker?
        if (line.rfind("#.", 0) == 0) {
            if (line == "#.symtab") {
                in_symtab = true;
                continue;
            } else if (in_symtab) {
                // we reached the next section -> stop
                break;
            }
        }

        if (!in_symtab) continue; // skip until we reach #.symtab

        // parse symbol table entry line
        std::istringstream ls(line);

        std::string index_colon;
        std::string value_hex;
        int dummy;
        std::string bind;     // GLOB or LOC
        std::string symbol;
        std::string section;

        ls >> index_colon;    // "0:"
        if (index_colon.back() == ':')
            index_colon.pop_back();

        if (!(ls >> value_hex >> dummy >> bind >> symbol))
            continue;
        ls >> section; // optional

        int value = 0;
        try {
            value = std::stoi(value_hex, nullptr, 16);
        } catch (...) {
            std::cerr << "Invalid value '" << value_hex << "' in line: " << line << "\n";
            continue;
        }

        bool local = (bind == "LOC");
        bool found = true;

        table.createEntry(value, found, local, symbol, section);
    }

    return table;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <symtab_file>\n";
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

    SymbolTable table = SymbolTable::from_string(content);

    // For testing: print the table back
    std::cout << table.to_string();

    return 0;
}
