#include <algorithm>
#include <iostream>
#include <list>

#include <assembler.h>

int usage(char const *name) {
  std::cout << "Usage:" << name
            << " [path/to/source/program.smc] ( -d | --debug | -t | --trace)\n";
  return 1;
}

int main(int argc, char const *argv[]) {
  Assembler a;

  std::list<std::string> args;
  args.push_back("-d");
  args.push_back("-t");
  args.push_back("--debug");
  args.push_back("--trace");

  if (argc == 1) return usage(argv[0]);

  if (argc == 2) {
    /*TODO check the file */
    return a.loadFile(argv[1]);
  }

  if (argc == 3) {
    auto it = std::find(args.begin(), args.end(), argv[2]);
    if (it != args.end()) {
      a.enableTrace();
      return a.loadFile(argv[1]);
    }

    std::cout << "Invalid 2nd argument\n";
    return usage(argv[0]);
  }

  std::cout << "Expected 1 or 2 arguments but got " << (argc - 1)
            << " instead.\n";
  return usage(argv[0]);
}
