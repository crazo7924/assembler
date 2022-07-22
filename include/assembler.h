#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

class Assembler {
private:
  bool trace = false;

public:
  Assembler();

  int loadFile(char const *file);
  int assemble(void);
  void enableTrace(void);
  int saveToFile(char const *file);
};

#endif
