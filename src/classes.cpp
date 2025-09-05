#include "classes.hpp"

std::string SymbolTable::to_string() const {
  std::ostringstream out;

  out << "#.symtab\n";
  out << "Num Value    Size Type  Bind Ndx Name\n";

  for (size_t i = 0; i < list.size(); ++i) {
    const auto &entry = list[i];

    out << std::dec << i << ": "
      << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << entry.value
      << " "
      << std::dec << 0
      << " ";

    out << "NOTYP ";

    out << (entry.local ? "LOC " : "GLOB ");

    out << entry.symbol << "\n";
  }

  return out.str();
}

std::string Section::section_to_string() const {
  std::ostringstream out;

  out << "#." << name << "\n";

  for (uint32_t i = 0; i < size; i++) {
    out << std::hex << std::setfill('0') << std::setw(2)
      << static_cast<int>(array[i]);

    if ((i % 16) != 15 && i + 1 < size)
      out << " ";

    if ((i % 16) == 15)
      out << "\n";
  }

  if (size % 16 != 0)
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
    out << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << rel.offset << " ";
    out << rel.symbol << " ";
    out << std::dec << rel.addend << "\n";
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

std::string FillTable::to_string() const {
  std::ostringstream out;

  out << "#.filltable\n";
  out << "Num Section   Offset   Symbol\n";

  for (size_t i = 0; i < list.size(); i++) {
    const auto &entry = list[i];

    uintptr_t offset = 0;
    if (entry.section && entry.sectionEntry) {
      offset = static_cast<uintptr_t>(entry.sectionEntry - entry.section->array);
    }

    out << std::setw(3) << std::dec << i << " ";
    out << std::setw(8) << (entry.section ? entry.section->name : "NULL") << " ";
    out << "0x" << std::setw(6) << std::setfill('0') << std::hex << offset << " ";
    out << entry.symbol << "\n";
  }

  return out.str();
}
