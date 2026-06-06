#ifndef __DEFS_H__
#define __DEFS_H__

#include <map>

enum class InstructionCode {
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

const std::map<InstructionCode, const char *> instructions = {
    {InstructionCode::I_STOP, "STOP"},
    {InstructionCode::I_ADD, "ADD"},
    {InstructionCode::I_SUB, "SUB"},
    {InstructionCode::I_MULT, "MULT"},
    {InstructionCode::I_DIV, "DIV"},
    {InstructionCode::I_MOVER, "MOVER"},
    {InstructionCode::I_MOVEM, "MOVEM"},
    {InstructionCode::I_COMP, "COMP"},
    {InstructionCode::I_BC, "BC"},
    {InstructionCode::I_READ, "READ"},
    {InstructionCode::I_PRINT, "PRINT"}
};

enum class ConditionCode {
    C_LT = 0,
    C_LE = 1,
    C_EQ = 2,
    C_GT = 3,
    C_GE = 4,
    C_ANY = 5
};

const std::map<ConditionCode, const char *> conditions = {
    {ConditionCode::C_LT, "LT"},
    {ConditionCode::C_LE, "LE"},
    {ConditionCode::C_EQ, "EQ"},
    {ConditionCode::C_GT, "GT"},
    {ConditionCode::C_GE, "GE"},
    {ConditionCode::C_ANY, "ANY"}
};

enum class RegisterCode {
    R_R0 = 0,
    R_R1 = 1,
    R_R2 = 2,
    R_R3 = 3
};

const std::map<RegisterCode, const char *> registers = {
    {RegisterCode::R_R0, "R0"},
    {RegisterCode::R_R1, "R1"},
    {RegisterCode::R_R2, "R2"},
    {RegisterCode::R_R3, "R3"}
};

enum class DirectiveCode {
    D_START = 1,
    D_END = 2,
    D_DS = 3,
    D_DC = 4
};

const std::map<DirectiveCode, const char *> directives = {
    {DirectiveCode::D_START, "START"},
    {DirectiveCode::D_END, "END"},
    {DirectiveCode::D_DS, "DS"},
    {DirectiveCode::D_DC, "DC"}
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
