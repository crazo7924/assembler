#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <assembler.h>

int main() {
    int failed = 0;

    auto run_test = [&](const std::string& name, const std::string& file, int expected_ret, const std::string& expected_output_contains) {
        std::cout << "Running test: " << name << " (" << file << ")... ";

        // Redirect std::cerr to a stringstream
        std::stringstream buffer;
        std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());

        Assembler a;
        if (a.loadFile(file.c_str()) == 0) {
            int ret = a.assemble();
            a.saveToFile("out.obj"); // Write output so it prints to cerr if any errors

            std::cerr.rdbuf(old_cerr); // Restore std::cerr
            std::string stderr_output = buffer.str();

            std::string out_obj_content;
            std::ifstream out_file("out.obj");
            if (out_file.is_open()) {
                std::stringstream obuf;
                obuf << out_file.rdbuf();
                out_obj_content = obuf.str();
            }

            if (ret == expected_ret) {
                bool out_matches = true;
                if (!expected_output_contains.empty()) {
                    if (expected_ret == 0) {
                         if (out_obj_content.find(expected_output_contains) == std::string::npos) out_matches = false;
                    } else {
                         if (stderr_output.find(expected_output_contains) == std::string::npos) out_matches = false;
                    }
                }

                if (out_matches) {
                    std::cout << "PASS\n";
                } else {
                    std::cout << "FAIL (Output did not contain expected string: " << expected_output_contains << ")\n";
                    failed++;
                }
            } else {
                std::cout << "FAIL (Expected return code " << expected_ret << ", got " << ret << ")\n";
                failed++;
            }
        } else {
            std::cerr.rdbuf(old_cerr);
            std::cout << "FAIL (Failed to load file)\n";
            failed++;
        }
    };

    run_test("Valid program", "tests/valid.smc", 0, "0100 09 0 S 0107");
    run_test("Undefined symbol", "tests/undefined_symbol.smc", 1, "Used but not defined");
    run_test("Corrupt syntax", "tests/corrupt.smc", 1, "");
    run_test("Empty program", "tests/empty.smc", 0, "--- Intermediate Code ---");
    run_test("Missing END", "tests/missing_end.smc", 0, "--- Intermediate Code ---");

    if (failed == 0) {
        std::cout << "All tests passed successfully.\n";
        return 0;
    } else {
        std::cout << failed << " test(s) failed.\n";
        return 1;
    }
}
