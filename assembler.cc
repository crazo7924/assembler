#include <assembler.h>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <map>
#include <vector>

ProcessStatus DirectiveProcessor::process(AssemblerContext& ctx, DirectiveCode dir_code) {
  if (dir_code == DirectiveCode::D_START) {
    if ((ctx.token_idx + 1) < ctx.tokens.size()) {
      ctx.LC = std::stoi(ctx.tokens[ctx.token_idx + 1]);
    }
  } else if (dir_code == DirectiveCode::D_END) {
    return ProcessStatus::STOP;
  } else if (dir_code == DirectiveCode::D_DS) {
    int size = std::stoi(ctx.tokens[ctx.token_idx + 1]);
    for (int k = 0; k < size; ++k) {
      ICTable entry;
      entry.address = ctx.LC++;
      entry.code = -1; // -1 for data/storage
      entry.reg = 0;
      entry.type = false;
      entry.value = 0;
      ctx.ic.push_back(entry);
    }
  } else if (dir_code == DirectiveCode::D_DC) {
    ICTable entry;
    entry.address = ctx.LC++;
    entry.code = -1;
    entry.reg = 0;
    entry.type = false;
    entry.value = std::stoi(ctx.tokens[ctx.token_idx + 1]);
    ctx.ic.push_back(entry);
  }
  return ProcessStatus::CONTINUE;
}

ProcessStatus InstructionProcessor::process(AssemblerContext& ctx,
                                            InstructionCode inst_code,
                                            const std::map<std::string, RegisterCode>& reg_map,
                                            const std::map<std::string, ConditionCode>& cond_map) {
  ICTable entry;
  entry.address = ctx.LC++;
  entry.code = static_cast<int>(inst_code);
  entry.reg = 0;
  entry.type = false;
  entry.value = 0;

  if ((ctx.token_idx + 1) < ctx.tokens.size()) {
    std::string op1 = ctx.tokens[ctx.token_idx + 1];

    if (op1.back() == ',') op1.pop_back();

    if (inst_code == InstructionCode::I_STOP) {
      // No operands
    } else if (inst_code == InstructionCode::I_BC) {
      if (cond_map.find(op1) != cond_map.end()) {
        entry.reg = static_cast<int>(cond_map.at(op1));
      }
      if ((ctx.token_idx + 2) < ctx.tokens.size()) {
        std::string op2 = ctx.tokens[ctx.token_idx + 2];
        bool found = false;
        for (auto& s : ctx.symtab) {
          if (std::string(s.symbol) == op2) {
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
          ctx.symtab.push_back(sym);
          entry.type = true;
          entry.value = 0;
        }
      }
    } else if (inst_code == InstructionCode::I_READ || inst_code == InstructionCode::I_PRINT) {
      std::string op2 = op1;
      bool found = false;
      for (auto& s : ctx.symtab) {
        if (std::string(s.symbol) == op2) {
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
        ctx.symtab.push_back(sym);
        entry.type = true;
        entry.value = 0;
      }
    } else {
      if (reg_map.find(op1) != reg_map.end()) {
        entry.reg = static_cast<int>(reg_map.at(op1));
      }
      if ((ctx.token_idx + 2) < ctx.tokens.size()) {
        std::string op2 = ctx.tokens[ctx.token_idx + 2];
        bool found = false;
        for (auto& s : ctx.symtab) {
          if (std::string(s.symbol) == op2) {
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
          ctx.symtab.push_back(sym);
          entry.type = true;
          entry.value = 0;
        }
      }
    }
  }
  ctx.ic.push_back(entry);
  return ProcessStatus::CONTINUE;
}



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

void Assembler::pass1(const std::map<std::string, InstructionCode>& inst_map,
                      const std::map<std::string, DirectiveCode>& dir_map) {
  int LC = 0;
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
        if (std::string(s.symbol) == first_token) {
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
      DirectiveCode dir_code = dir_map.at(op);
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
}

ProcessStatus Assembler::pass2(const std::map<std::string, InstructionCode>& inst_map,
                      const std::map<std::string, DirectiveCode>& dir_map,
                      const std::map<std::string, RegisterCode>& reg_map,
                      const std::map<std::string, ConditionCode>& cond_map) {
  int LC = 0;
  DirectiveProcessor dir_proc;
  InstructionProcessor inst_proc;

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
    AssemblerContext ctx{LC, ic, symtab, tokens, token_idx};

    if (dir_map.find(op) != dir_map.end()) {
      DirectiveCode dir_code = dir_map.at(op);
      ProcessStatus status = dir_proc.process(ctx, dir_code);
      if (status == ProcessStatus::STOP) {
        break; // D_END or error handled
      }
    } else if (inst_map.find(op) != inst_map.end()) {
      InstructionCode inst_code = inst_map.at(op);
      ProcessStatus status = inst_proc.process(ctx, inst_code, reg_map, cond_map);
      if (status == ProcessStatus::STOP) {
        break;
      }
    }
  }

  return ProcessStatus::CONTINUE;
}

void Assembler::checkUndefinedSymbols() {
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
}

int Assembler::assemble() {
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

  pass1(inst_map, dir_map);
  ProcessStatus pass2_status = pass2(inst_map, dir_map, reg_map, cond_map);
  if (pass2_status == ProcessStatus::ERROR) {
    return 1;
  }
  checkUndefinedSymbols();

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
