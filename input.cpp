#include "cvm.h"
#include <cstdlib>
#include <cstdio>


using namespace cvm;

CInput::Function::~Function() {
	delete[] instructions;
}

CInput::CInput() :
	staticvar_num(0),
	staticvars(nullptr),
	function_num(0),
	functions(nullptr)
{

}

CInput::~CInput() {
	delete[] staticvars;
	delete[] functions;
}

int CInput::readfile(const char* filename) {
	FILE* file = fopen(filename, "r");
	
	if (!file)return ERR_INPUT_FILEOPEN;
	
	// section-0: 
	if (!fread(&head, sizeof(Head), 1, file)) {
		return ERR_INPUT_FILEEND;
	}

	// section-1: static variables
	if (!fread(&staticvar_num, sizeof(staticvar_num), 1, file)) {
		return ERR_INPUT_FILEEND;
	}
	if (staticvar_num >= CINPUT_MAX_BUF)return ERR_INPUT_EXCESS_BUF;
	staticvars = new StaticVar[staticvar_num];
	
	for (size_t i = 0; i < staticvar_num; i++) {
		if (!fread(&staticvars[i].size, sizeof(size_t), 1, file)) {
			return ERR_INPUT_FILEEND;
		}
		if (staticvars[i].size > CINPUT_MAX_BUF)return ERR_INPUT_EXCESS_BUF;
		if (fread(&staticvars[i].val, sizeof(unit_t), staticvars[i].size, file) < staticvars[i].size) {
			return ERR_INPUT_FILEEND;
		}
	}


	// section-2: self-defined functions
	if (!fread(&function_num, sizeof(function_num), 1, file)) {
		return ERR_INPUT_FILEEND;
	}
	if (function_num >= CINPUT_MAX_BUF)return ERR_INPUT_EXCESS_BUF;
	functions = new Function[function_num];

	for (size_t i = 0; i < function_num; i++) {
		if (!fread(&functions[i].size, sizeof(size_t), 1, file)) {
			return ERR_INPUT_FILEEND;
		}
		if (functions[i].size > CINPUT_MAX_BUF)return ERR_INPUT_EXCESS_BUF;
		if (fread(functions[i].instructions, sizeof(instruction_t), functions[i].size, file) < functions[i].size) {
			return ERR_INPUT_FILEEND;
		}
	}
	fclose(file);
	return 0;
}

int cvm::CInput::writefile(const char* filename) {
	FILE* file = fopen(filename, "w");

	if (!file)return ERR_INPUT_FILEOPEN;

	fwrite(&head, sizeof(Head), 1, file);

	fwrite(&staticvar_num, sizeof(size_t), 1, file);
	for (size_t i = 0; i < staticvar_num; i++) {
		fwrite(&staticvars[i].size, sizeof(size_t), 1, file);
		fwrite(staticvars[i].val, sizeof(unit_t), staticvars[i].size, file);
	}

	fwrite(&function_num, sizeof(size_t), 1, file);
	for (size_t i = 0; i < function_num; i++) {
		fwrite(&functions[i].size, sizeof(size_t), 1, file);
		fwrite(functions[i].instructions, sizeof(instruction_t), functions[i].size, file);
	}

	fclose(file);
    return 0;
}
