/* CVM Assembler written in cpp*/

#include "assemble.h"
#include <fstream>

#include "assemble_util.h"
#include "memory.h"

using namespace std;

#define CVM_INSTRUCT_EX_BIN     0x1
#define CVM_INSTRUCT_EX_STACK   0x2
#define CVM_INSTRUCT_EX_IMDT    0x3

namespace cvm {


    const unordered_map<string, char> code_map = {
        { "hlt", CVM_INSTRUCT_HLT },
        { "movu", CVM_INSTRUCT_MOV },
        { "bmovu", CVM_INSTRUCT_MOV + (CVM_INSTRUCT_EX_BIN << 4)},
        { "copyu", CVM_INSTRUCT_COPY},
        { "scopyu", CVM_INSTRUCT_COPY + (CVM_INSTRUCT_EX_STACK << 4)},
        { "pushu", CVM_INSTRUCT_PUSH },
        { "spushu", CVM_INSTRUCT_PUSH + (CVM_INSTRUCT_EX_STACK << 4) },
        { "popu", CVM_INSTRUCT_POP },
        { "spopu", CVM_INSTRUCT_POP + (CVM_INSTRUCT_EX_STACK << 4) },
        { "jmp", CVM_INSTRUCT_JMP },
        { "je", CVM_INSTRUCT_JMP + (CVM_JMP_ZERO << 4) },
        { "jne", CVM_INSTRUCT_JMP + (CVM_JMP_NONZERO << 4) },
        { "jg", CVM_INSTRUCT_JMP + (CVM_JMP_POSITIVE << 4) },
        { "jge", CVM_INSTRUCT_JMP + (CVM_JMP_NONNEG << 4)},
        { "call", CVM_INSTRUCT_CALL },
        { "run", CVM_INSTRUCT_CALL + (CVM_INSTRUCT_EX_IMDT << 4) },
        { "leave", CVM_INSTRUCT_RET },
        { "ret", CVM_INSTRUCT_RET + (CVM_RET_PC << 4)},
        { "int", CVM_INSTRUCT_INT},
        { "addu", CVM_INSTRUCT_BINOP + (CVM_BINOP_ADD << 4)},
        { "subu", CVM_INSTRUCT_BINOP + (CVM_BINOP_SUB << 4) },
        { "mulu", CVM_INSTRUCT_BINOP + (CVM_BINOP_MUL << 4) },
        { "divu", CVM_INSTRUCT_BINOP + (CVM_BINOP_DIV << 4) },
        { "modu", CVM_INSTRUCT_BINOP + (CVM_BINOP_MOD << 4) },
        { "andu", CVM_INSTRUCT_BINOP + (CVM_BINOP_AND << 4) },
        { "oru", CVM_INSTRUCT_BINOP + (CVM_BINOP_OR << 4) },
        { "xoru", CVM_INSTRUCT_BINOP + (CVM_BINOP_XOR << 4) },
        { "passu", CVM_INSTRUCT_BINOP + (CVM_SINOP_PASS << 4) },
        { "incu", CVM_INSTRUCT_BINOP + (CVM_SINOP_ADDONE << 4) },
        { "decu", CVM_INSTRUCT_BINOP + (CVM_SINOP_SUBONE << 4) },
        { "notu", CVM_INSTRUCT_BINOP + (CVM_SINOP_NOT << 4) },
        { "get", CVM_INSTRUCT_MBR + (CVM_MBR_GETMB << 4) },
        { "set", CVM_INSTRUCT_MBR + (CVM_MBR_SETMB << 4) }
    };


    struct Segment {

        enum Type
        {
            EMPTY,
            VAR,
            CODE
        };

        Type type;
        vector<string> code;
    };


    enum DataStyle
    {
        NONE,
        PLAIN, // plain data
        IMDT,  // $data
        REF,   // (data)
        REG    // %data
    };

    DataStyle get_datastyle(const string& data) {
        if (data.front() == '$') { 
            return DataStyle::IMDT; 
        }
        else if (data.front() == '(') {
            return DataStyle::REF;
        }
        else if (data.front() == '%') {
            return DataStyle::REG;
        }
        else {
            return DataStyle::PLAIN;
        }
    }

    char get_memcode(char code, DataStyle data_style) {
        switch (data_style)
        {
        case NONE:
            if ((code & 0xf) == CVM_INSTRUCT_BINOP) {
                return CVM_INSTRUCT_MEM_LOCAL;
            }
        case PLAIN:
            switch (code >> 4)
            {
            case CVM_INSTRUCT_EX_BIN:           // bmov
                return CVM_INSTRUCT_MEM_LOCAL;

            case CVM_INSTRUCT_EX_STACK:         // scopy, spush, spop
                return CVM_INSTRUCT_MEM_OFFSET;

            case CVM_INSTRUCT_EX_IMDT:          // run
                return CVM_INSTRUCT_MEM_IMDT;

            default:
                return CVM_INSTRUCT_MEM_ADDR;
            }
        case IMDT:
            if ((code & 0xf) == CVM_INSTRUCT_JMP || (code & 0xf) == CVM_INSTRUCT_CALL) {
                return CVM_INSTRUCT_MEM_OFFSET;
            }
            else {
                return CVM_INSTRUCT_MEM_IMDT;
            }
        case REF:
        case REG:
            return CVM_INSTRUCT_MEM_REG;
        default:    // won't actually happen
            return 0;
        }
    }

    short try_replace_token(const string& text, const AddrMap& jmp_table, bool is_seg) {
        if (is_digit(text)) {
            return (short)atoi(text.c_str());
        }
        else {
            return is_seg ? jmp_table.at(text).seg : jmp_table.at(text).offset;
        }
    }

    short get_data(const string& text, DataStyle ds, bool is_seg, const AddrMap& jmp_table) {

        switch (ds)
        {
        case PLAIN:
            return try_replace_token(text, jmp_table, is_seg);
        case IMDT:
            return try_replace_token(string(text.begin() + 1, text.end()), jmp_table, is_seg);
        case REF:
            return try_replace_token(string(text.begin() + 1, text.end() - 1), jmp_table, is_seg);
        case REG:
            if (text == "%esp") {
                return ADDR_STACK_PTR;
            }
            else if (text == "%ebp") {
                return ADDR_FRAME_PTR;
            }
            else if (text == "%pc") {
                return ADDR_PC;
            }
            else if (text == "%ret") {
                return ADDR_RESULT;
            }
            else {

            }
        case NONE:
            return 0;
        default:
            break;
        }
        return 0;
    }

    instruction_t translate_code(const string& line, const AddrMap& jmp_table) {
        vector<string> line_split = split(line, ' ');
        instruction_t ins;
        ins.code = code_map.at(line_split[0]);

        DataStyle ds;
        if (line_split.size() > 1) {
            ds = get_datastyle(line_split[1]);
        }
        else {
            ds = DataStyle::NONE;
        }

        ins.mem_code = get_memcode(ins.code, ds);

        if (line_split.size() > 1) {
            bool is_seg = true;
            if (((ins.code & 0xf) == CVM_INSTRUCT_JMP || (ins.code & 0xf) == CVM_INSTRUCT_CALL) && 
                ins.mem_code == CVM_INSTRUCT_MEM_OFFSET) {
                is_seg = false;
            }

            ins.data = get_data(line_split[1], ds, is_seg, jmp_table);
        }
        else {
            ins.data = 0;
        }
        return ins;
    }

    void set_seg_attr(const vector<string>& cmd, Segment& seg) {
        if (cmd[0] == "data") {
            seg.type = Segment::VAR;
        }
        else if (cmd[0] == "code") {
            seg.type = Segment::CODE;
        }
        else {
            throw AsmError("Invalid attribute: " + cmd[0]);
        }
    }

    void translate_var(const vector<string>& code, CInput::StaticVar& var) {

        vector<string> cmd_split = split(code[0], ' ');

        if (cmd_split[0] == "unit") {
            var.val = new unit_t[1];
            var.size = 1;
            var.val[0] = (unit_t)atoi(cmd_split[1].c_str());
        }
        else {
            throw AsmError("Invalid variable type: " + code[0]);
        }
    }

    void translate_func(const vector<string>& code, const AddrMap& jmp_table, CInput::Function& func) {
        func.size = code.size();
        func.instructions = new instruction_t[func.size];
        for (size_t i = 0; i < func.size; i++) {
            func.instructions[i] = translate_code(code[i], jmp_table);
        }
    }

    // read segments from lines
    vector<Segment> read_segments(const vector<string>& lines, AddrMap& segment_jmp_table) {
        vector<Segment> segments;

        for (const auto& line : lines) {
            if (starts_with(line, "section")) {
                string seg_name = split(line, ' ')[1];
                segment_jmp_table.insert({ seg_name, {(int)segments.size(), 0} });
                segments.push_back(Segment());
            }
            else {
                segments.back().code.push_back(line);
            }
        }
        return segments;
    }

    // construct jump table from labels
    void read_labels(Segment& segment, int seg_addr, AddrMap& jmp_table) {

        int cur_addr = 0;

        for (auto line = segment.code.begin(); line != segment.code.end();) {
            if (line->back() == ':') {
                jmp_table.insert({ string(line->begin(), line->end() - 1), {seg_addr, cur_addr} });
                line = segment.code.erase(line);
            }
            else if (line->front() == '.') {
                set_seg_attr(split(string(line->begin() + 1, line->end()), ' '), segment);
                line = segment.code.erase(line);
            }
            else {
                cur_addr++;
                line++;
            }

        }
    }

    // compile segments into input
    void compile_segments(const vector<Segment>& segments, const AddrMap& jmp_table, CInput& input) {

        for (const auto& seg : segments) {
            if (seg.type == Segment::VAR) {
                input.staticvar_num++;
            }
            else if (seg.type == Segment::CODE) {
                input.function_num++;
            }
        }

        input.staticvars = new CInput::StaticVar[input.staticvar_num];
        input.functions = new CInput::Function[input.function_num];

        int staticvar_cnt = 0;
        int function_cnt = 0;

        for (const auto& seg : segments) {
            if (seg.type == Segment::VAR) {
                translate_var(seg.code, input.staticvars[staticvar_cnt++]);
            }
            else if (seg.type == Segment::CODE) {
                translate_func(seg.code, jmp_table, input.functions[function_cnt++]);
            }
        }

        try {
            input.head.entry = jmp_table.at(MAIN_ENTRY).linarize();
        }
        catch (exception) {
            throw AsmError("No head entry");
        }

    }


    int readlines(const vector<string>& lines, CInput& input) {
        AddrMap jmp_table;
        vector<Segment> segments = read_segments(lines, jmp_table);

        // read other file here

        // read built-in functions here

        int seg_base = CONST_BEGIN + input.nativefunc_num;

        for (int i = 0; i < segments.size(); i++) {
            read_labels(segments[i], seg_base + i, jmp_table);
        }

        compile_segments(segments, jmp_table, input);

        return 0;

    }

    // read casm file
    int readfile(const string & filename, CInput& input)
    {
        if (input.staticvars || input.functions) {
            throw AsmError("input not empty");
            return 1;
        }


        vector<string> lines;

        ifstream infile(filename);

        if (!infile.is_open()) {
            throw AsmError("Invalid file");
        }

        char buffer[256];

        while (!infile.eof()) {
            infile.getline(buffer, 256);
            string line = trim(remove_comment(string(buffer)));
            if (!line.empty()) {
                lines.push_back(move(line));
            }
        }

        input.staticvar_num = 0;
        input.function_num = 0;
        input.nativefunc_num = 0;

        return readlines(lines, input);
    }

}