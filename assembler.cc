#include <assembler.h>

#include <fstream>
#include <iostream>
#include <string>

Assembler::Assembler() {}

int Assembler::assemble() { /*TODO*/
  return 0;
}

int Assembler::saveToFile(char const *name) { /*TODO*/
  return 0;
}

int Assembler::loadFile(const char *name) {
  std::ifstream file;
  if (file.is_open()) {
    std::cerr << " File already open(!?)\n";
    return 1;
  }

  file.open(name);

  while (file.peek() != EOF) {
    std::string line;
    std::getline(file, line);
    // tokenize(line);
  }
  file.close();
  return 0;
}

void Assembler::enableTrace() {
  this->trace = true;
  std::cout << "[[ Trace mode enabled ]]\n";
}
