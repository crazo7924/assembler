#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

class Simulator {
  int memory[1000]; // addresses [0-999] as RAM
  int pc;           // program counter
  int registers[4]; // four general purpose registers
  int cc;           // condition code
  int lc;           // the last valid address
};

#endif // __SIMULATOR_H__
