#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

#include <vector>
#include <string>
#include "defs.h"

class Assembler {
private:
  bool trace = false;

  std::vector<std::vector<std::string>> parsed_lines;
  std::vector<SymbolTable> symtab;
  std::vector<ICTable> ic;
  std::vector<ErrorTable> errors;

  void tokenize(const std::string& line);
  void pass1(const std::map<std::string, InstructionCode>& inst_map,
             const std::map<std::string, DirectiveCode>& dir_map);
  void pass2(const std::map<std::string, InstructionCode>& inst_map,
             const std::map<std::string, DirectiveCode>& dir_map,
             const std::map<std::string, RegisterCode>& reg_map,
             const std::map<std::string, ConditionCode>& cond_map);
  void checkUndefinedSymbols();

public:
  Assembler();

  int loadFile(char const *file);
  int assemble(void);
  void enableTrace(void);
  int saveToFile(char const *file);
};

#endif
