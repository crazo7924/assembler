#ifndef __DEFS_H__
#define __DEFS_H__

#include <map>

const std::map<int, const char *> instructions = {
    {0, "STOP"},  // Stop or halt execution
    {1, "ADD"},   // Add memory operand to register operand
    {2, "SUB"},   // Subtract memory operand from register operand
    {3, "MULT"},  // Multiply memory operand to register operand
    {4, "DIV"},   // Divide memory operand by register operand
    {5, "MOVER"}, // Move memory operand contents to register operand
    {6, "MOVEM"}, // Move register operand contents to memory operand
    {7, "COMP"},  // Compare register and memory operands to set condition code
                  // appropriately
    {8, "BC"}, // Branch to second operand depending on condition code specified
               // as first operand
    {9, "READ"},  // Read into memory operand
    {10, "PRINT"} // Print contents of memory Operand
};

const std::map<int, const char *> conditions = {
    {0, "LT"}, // '<' less than
    {1, "LE"}, // '<=' less than or equal
    {2, "EQ"}, // '==' equal
    {3, "GT"}, // '>' greater than
    {4, "GE"}, // '>=' greater than or equal
    {5, "ANY"} // unconditional
};

constexpr auto errors = {
    "Used but not defined"
    // TODO: add more errors
};

typedef const struct {
  char symbol[20]; // the current symbol
  int address;     // memory location of this symbol
  bool used;       //  wether it is used
  bool defined;    // wether it is defined
} SymbolTable;

typedef const struct {
  int address; // location in the memory
  int code;    // operation code aka opcode
  int reg;     // register operand
  bool type;   // symbol (true) or constant (false)
  int value;   // the actual contents
} ICTable;

typedef const struct {
  int line;  // line number
  int error; // error code
} ErrorTable;

#endif // __DEFS_H__
