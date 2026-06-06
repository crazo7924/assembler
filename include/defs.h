#ifndef __DEFS_H__
#define __DEFS_H__

#include <map>

enum InstructionCode {
    I_STOP = 0,
    I_ADD = 1,
    I_SUB = 2,
    I_MULT = 3,
    I_DIV = 4,
    I_MOVER = 5,
    I_MOVEM = 6,
    I_COMP = 7,
    I_BC = 8,
    I_READ = 9,
    I_PRINT = 10
};

const std::map<int, const char *> instructions = {
    {I_STOP, "STOP"},
    {I_ADD, "ADD"},
    {I_SUB, "SUB"},
    {I_MULT, "MULT"},
    {I_DIV, "DIV"},
    {I_MOVER, "MOVER"},
    {I_MOVEM, "MOVEM"},
    {I_COMP, "COMP"},
    {I_BC, "BC"},
    {I_READ, "READ"},
    {I_PRINT, "PRINT"}
};

enum ConditionCode {
    C_LT = 0,
    C_LE = 1,
    C_EQ = 2,
    C_GT = 3,
    C_GE = 4,
    C_ANY = 5
};

const std::map<int, const char *> conditions = {
    {C_LT, "LT"},
    {C_LE, "LE"},
    {C_EQ, "EQ"},
    {C_GT, "GT"},
    {C_GE, "GE"},
    {C_ANY, "ANY"}
};

enum RegisterCode {
    R_R0 = 0,
    R_R1 = 1,
    R_R2 = 2,
    R_R3 = 3
};

const std::map<int, const char *> registers = {
    {R_R0, "R0"},
    {R_R1, "R1"},
    {R_R2, "R2"},
    {R_R3, "R3"}
};

enum DirectiveCode {
    D_START = 1,
    D_END = 2,
    D_DS = 3,
    D_DC = 4
};

const std::map<int, const char *> directives = {
    {D_START, "START"},
    {D_END, "END"},
    {D_DS, "DS"},
    {D_DC, "DC"}
};

const char* const errors[] = {
    "Used but not defined"
    // TODO: add more errors
};

typedef struct {
  char symbol[20]; // the current symbol
  int address;     // memory location of this symbol
  bool used;       //  wether it is used
  bool defined;    // wether it is defined
} SymbolTable;

typedef struct {
  int address; // location in the memory
  int code;    // operation code aka opcode
  int reg;     // register operand
  bool type;   // symbol (true) or constant (false)
  int value;   // the actual contents
} ICTable;

typedef struct {
  int line;  // line number
  int error; // error code
} ErrorTable;

#endif // __DEFS_H__
