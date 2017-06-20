#pragma once

#ifndef CVM_TYPES
#define CVM_TYPES

#include <stdint.h>

namespace cvm {

#define TYPE_UNKNOWN		0x00
#define TYPE_VAR			0x01
#define TYPE_FUNC			0x02
#define TYPE_NATIVE_FUNC	0x03

#define UNIT_LENGTH			32
#define PTR_LENGTH			32
#define SEG_SIZE			16
#define SEG_SIZE_MASK		0xffff

#define INSTRUCT_LENGTH		32
#define INSTRUCT_CODE_LENGTH	8
#define INSTRUCT_TYPECODE_LENGTH	4
#define INSTRUCT_SUBCODE_LENGTH		4

	typedef uint32_t unit_t, pointer_t, *unit_ptr;
	typedef unit_t vartype_t;
	typedef void* native_ptr;

	struct Object{
		vartype_t type;
		unit_t size;
		unit_ptr val;
	};

	typedef Object* obj_ptr;

	struct NativeFunc {
		unit_t argc;
		unit_t arg_descriptor; /* bit vector,
							   determine wheter argument is simple (0) or object (1)*/
		unit_t returns;
		int (*func_ptr)(native_ptr*, unit_ptr); // func(**argv, *return)
	};

	struct instruction_t {
		char code;
		char mem_code;
		short data;
	};
}


#endif // !CVM_TYPES
