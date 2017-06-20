#pragma once
#include "types.h"
#include "instruction.h"

#define CINPUT_MAX_BUF			0x10000


namespace cvm {
	class CInput
	{
	public:
		typedef unit_t BuiltFunc;

		CInput();
		~CInput();

		struct Head {
			unit_t entry;
		};
		Head head;

		struct StaticVar
		{
			size_t size;
			unit_ptr val;
		};

		StaticVar* staticvars;		
		size_t staticvar_num;

		NativeFunc* nativefuncs;
		size_t nativefunc_num;

		struct Function {
			size_t size;
			instruction_t* instructions;
			~Function();
		};

		Function* functions;
		size_t function_num;

		int readfile(const char* filename);
		int writefile(const char* filename);

		/*
		Input format:

		imovu  0x0200;
		||  |  |	|
		01  4  7    a
		*/


	};
}
