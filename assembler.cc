#include <assembler.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <map>
#include <vector>

Assembler::Assembler() {}

void Assembler::tokenize(const std::string& line) {
  std::string code_part = line;
  size_t comment_pos = code_part.find(';');
  if (comment_pos != std::string::npos) {
    code_part = code_part.substr(0, comment_pos);
  }

  std::vector<std::string> tokens;
  std::stringstream ss(code_part);
  std::string token;
  while (ss >> token) {
    tokens.push_back(token);
  }

  if (!tokens.empty()) {
    parsed_lines.push_back(tokens);
  }
}

int Assembler::loadFile(const char *name) {
  std::ifstream file;
  file.open(name);
  if (!file.is_open()) {
    std::cerr << " Failed to open file: " << name << "\n";
    return 1;
  }

  while (file.peek() != EOF) {
    std::string line;
    std::getline(file, line);
    tokenize(line);
  }
  file.close();
  return 0;
}

int Assembler::assemble() {
  int LC = 0;

  std::map<std::string, InstructionCode> inst_map;
  for (const auto& pair : instructions) {
    inst_map[pair.second] = pair.first;
  }

  std::map<std::string, RegisterCode> reg_map;
  for (const auto& pair : registers) {
    reg_map[pair.second] = pair.first;
  }

  std::map<std::string, ConditionCode> cond_map;
  for (const auto& pair : conditions) {
    cond_map[pair.second] = pair.first;
  }

  std::map<std::string, DirectiveCode> dir_map;
  for (const auto& pair : directives) {
    dir_map[pair.second] = pair.first;
  }

  // Pass 1: Build Symbol Table
  for (size_t i = 0; i < parsed_lines.size(); ++i) {
    const auto& tokens = parsed_lines[i];
    if (tokens.empty()) continue;

    size_t token_idx = 0;

    std::string first_token = tokens[0];
    bool is_label = false;

    if (first_token.back() == ':') {
      is_label = true;
      first_token.pop_back();
    } else if (inst_map.find(first_token) == inst_map.end() && dir_map.find(first_token) == dir_map.end()) {
      is_label = true;
    }

    if (is_label) {
      SymbolTable sym;
      std::strncpy(sym.symbol, first_token.c_str(), sizeof(sym.symbol) - 1);
      sym.symbol[sizeof(sym.symbol) - 1] = '\0';
      sym.address = LC;
      sym.defined = true;
      sym.used = false;

      bool exists = false;
      for (auto& s : symtab) {
        if (first_token == s.symbol) {
          s.defined = true;
          s.address = LC;
          exists = true;
          break;
        }
      }
      if (!exists) {
        symtab.push_back(sym);
      }
      token_idx++;
    }

    if (token_idx >= tokens.size()) continue;

    std::string op = tokens[token_idx];
    if (dir_map.find(op) != dir_map.end()) {
      DirectiveCode dir_code = dir_map[op];
      if (dir_code == DirectiveCode::D_START) {
        if ((token_idx + 1) < tokens.size()) {
          LC = std::stoi(tokens[token_idx + 1]);
        }
      } else if (dir_code == DirectiveCode::D_END) {
        break;
      } else if (dir_code == DirectiveCode::D_DS) {
        if ((token_idx + 1) < tokens.size()) {
          LC += std::stoi(tokens[token_idx + 1]);
        }
      } else if (dir_code == DirectiveCode::D_DC) {
        LC += 1;
      }
    } else if (inst_map.find(op) != inst_map.end()) {
      LC += 1;
    }
  }

  // Pass 2: Generate Intermediate Code
  LC = 0;
  for (size_t i = 0; i < parsed_lines.size(); ++i) {
    const auto& tokens = parsed_lines[i];
    if (tokens.empty()) continue;

    size_t token_idx = 0;

    std::string first_token = tokens[0];
    if (first_token.back() == ':') {
      token_idx++;
    } else if (inst_map.find(first_token) == inst_map.end() && dir_map.find(first_token) == dir_map.end()) {
      token_idx++;
    }

    if (token_idx >= tokens.size()) continue;

    std::string op = tokens[token_idx];
    if (dir_map.find(op) != dir_map.end()) {
      DirectiveCode dir_code = dir_map[op];
      if (dir_code == DirectiveCode::D_START) {
        if ((token_idx + 1) < tokens.size()) {
          LC = std::stoi(tokens[token_idx + 1]);
        }
      } else if (dir_code == DirectiveCode::D_END) {
        break;
      } else if (dir_code == DirectiveCode::D_DS) {
        int size = std::stoi(tokens[token_idx + 1]);
        for (int k = 0; k < size; ++k) {
          ICTable entry;
          entry.address = LC++;
          entry.code = -1; // -1 for data/storage
          entry.reg = 0;
          entry.type = false;
          entry.value = 0;
          ic.push_back(entry);
        }
      } else if (dir_code == DirectiveCode::D_DC) {
        ICTable entry;
        entry.address = LC++;
        entry.code = -1;
        entry.reg = 0;
        entry.type = false;
        entry.value = std::stoi(tokens[token_idx + 1]);
        ic.push_back(entry);
      }
    } else if (inst_map.find(op) != inst_map.end()) {
      InstructionCode inst_code = inst_map[op];
      ICTable entry;
      entry.address = LC++;
      entry.code = static_cast<int>(inst_code);
      entry.reg = 0;
      entry.type = false;
      entry.value = 0;

      if ((token_idx + 1) < tokens.size()) {
        std::string op1 = tokens[token_idx + 1];

        if (op1.back() == ',') op1.pop_back();

        if (inst_code == InstructionCode::I_STOP) {
          // No operands
        } else if (inst_code == InstructionCode::I_BC) {
          if (cond_map.find(op1) != cond_map.end()) {
            entry.reg = static_cast<int>(cond_map[op1]);
          }
          if ((token_idx + 2) < tokens.size()) {
            std::string op2 = tokens[token_idx + 2];
            bool found = false;
            for (auto& s : symtab) {
              if (op2 == s.symbol) {
                entry.type = true;
                entry.value = s.address;
                s.used = true;
                found = true;
                break;
              }
            }
            if (!found) {
              SymbolTable sym;
              std::strncpy(sym.symbol, op2.c_str(), sizeof(sym.symbol) - 1);
              sym.symbol[sizeof(sym.symbol) - 1] = '\0';
              sym.address = 0;
              sym.defined = false;
              sym.used = true;
              symtab.push_back(sym);
              entry.type = true;
              entry.value = 0;
            }
          }
        } else if (inst_code == InstructionCode::I_READ || inst_code == InstructionCode::I_PRINT) {
          std::string op2 = op1;
          bool found = false;
          for (auto& s : symtab) {
            if (op2 == s.symbol) {
              entry.type = true;
              entry.value = s.address;
              s.used = true;
              found = true;
              break;
            }
          }
          if (!found) {
            SymbolTable sym;
            std::strncpy(sym.symbol, op2.c_str(), sizeof(sym.symbol) - 1);
            sym.symbol[sizeof(sym.symbol) - 1] = '\0';
            sym.address = 0;
            sym.defined = false;
            sym.used = true;
            symtab.push_back(sym);
            entry.type = true;
            entry.value = 0;
          }
        } else {
          if (reg_map.find(op1) != reg_map.end()) {
            entry.reg = static_cast<int>(reg_map[op1]);
          }
          if ((token_idx + 2) < tokens.size()) {
            std::string op2 = tokens[token_idx + 2];
            bool found = false;
            for (auto& s : symtab) {
              if (op2 == s.symbol) {
                entry.type = true;
                entry.value = s.address;
                s.used = true;
                found = true;
                break;
              }
            }
            if (!found) {
              SymbolTable sym;
              std::strncpy(sym.symbol, op2.c_str(), sizeof(sym.symbol) - 1);
              sym.symbol[sizeof(sym.symbol) - 1] = '\0';
              sym.address = 0;
              sym.defined = false;
              sym.used = true;
              symtab.push_back(sym);
              entry.type = true;
              entry.value = 0;
            }
          }
        }
      }
      ic.push_back(entry);
    }
  }

  for (const auto& s : symtab) {
    if (s.used && !s.defined) {
      ErrorTable err;
      err.line = 0;
      err.error = 0; // 0 matches errors[0]
      errors.push_back(err);
      if (trace) {
        std::cerr << "Error: Symbol '" << s.symbol << "' used but not defined.\n";
      }
    }
  }

  return errors.empty() ? 0 : 1;
}

int Assembler::saveToFile(char const *name) {
  std::ofstream file(name);
  if (!file.is_open()) {
    std::cerr << " Failed to open output file: " << name << "\n";
    return 1;
  }

  if (!errors.empty()) {
    std::cerr << "Errors encountered during assembly:\n";
    for (const auto& err : errors) {
      if (err.error >= 0 && static_cast<size_t>(err.error) < (sizeof(::errors)/sizeof(::errors[0]))) {
         std::cerr << ::errors[err.error] << " at line " << err.line << "\n";
      } else {
         std::cerr << "Error Code " << err.error << " at line " << err.line << "\n";
      }
    }
  }

  if (trace) {
    file << "--- Symbol Table ---\n";
    for (const auto& s : symtab) {
      file << "Symbol: " << std::setw(8) << s.symbol
           << " Address: " << std::setw(4) << s.address
           << " Defined: " << s.defined
           << " Used: " << s.used << "\n";
    }
    file << "\n";
  }

  file << "--- Intermediate Code ---\n";
  for (const auto& entry : ic) {
    file << std::setw(4) << std::setfill('0') << entry.address << " ";
    if (entry.code == -1) {
      // Data
      file << "DATA " << std::setw(4) << std::setfill('0') << entry.value << "\n";
    } else {
      // Instruction
      file << std::setw(2) << std::setfill('0') << entry.code << " ";
      file << entry.reg << " ";
      file << (entry.type ? "S" : "C") << " "
           << std::setw(4) << std::setfill('0') << entry.value << "\n";
    }
  }

  file.close();
  return 0;
}

void Assembler::enableTrace() {
  this->trace = true;
  std::cout << "[[ Trace mode enabled ]]\n";
}
