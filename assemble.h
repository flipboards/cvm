#pragma once

// calc-asm reader

#include "input.h"
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

#define MAIN_ENTRY  "__main"

namespace cvm {

    class AsmError {
    public:
        AsmError() : _what(nullptr) {

        }
        explicit AsmError(const string& str) : _what(str.c_str()) {

        }

        const char* what()const {
            return _what;
        }

    private:
        const char* _what;
    };

    struct address_pair {
        int seg;
        int offset;

        int linarize()const {
            return (seg << SEG_SIZE) + offset;
        }
    };

    typedef unordered_map<string, address_pair> AddrMap;

	int readfile(const string& filename, CInput& input);
    int readlines(const vector<string>& lines, CInput& input);

    void translate_var(const vector<string>& text, CInput::StaticVar& var);
    void translate_func(const vector<string>& text, const AddrMap& jmp_table, CInput::Function& func);

    instruction_t translate_code(const string& line, const AddrMap& jmp_table);

}
