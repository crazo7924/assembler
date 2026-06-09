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
  void findOrAddSymbol(const std::string& symbol_name, ICTable& entry);

public:
  Assembler();

  int loadFile(char const *file);
  int assemble(void);
  void enableTrace(void);
  int saveToFile(char const *file);
};

#endif
