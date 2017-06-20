#pragma once

#include "types.h"
#include "memory.h"
#include "input.h"

using namespace std;

namespace cvm {

	// Inner Address


	class CalcVM {
	public:
		CalcVM();
		~CalcVM();

		// load structured input
		int load(CInput* input);

		// run the VM
		int run();

		// execute one instruction
		int execute();

		// call a callable object (function/native function)
		int call(pointer_t func);

		// send interrupt to kernel (system calls)
		void interrupt(unit_t int_no);

		// raise an error
		void raise(unit_t error);

		// check the last error
		void check_error();

		// free all memory and exit
		int release(); 

		int status;
		unit_t ret;
	private:

		/*Instruction implementations*/

		// move data to pointer
		int _movu(instruction_t line);

		// move pointer to pointer
		int _copyu(instruction_t line);

		// move data to stack top, ret will not be set
		int _pushu(instruction_t line);

		// pop stack top to pointer
		int _popu(instruction_t line);

		// jump to pc/function pointer/offset
		int _jmpu(instruction_t line);

		// call pc/function pointer
		int _call(instruction_t line);

		// restore pc&frame pointer
		int _ret(instruction_t line);

		// binary operator
		int _binop(instruction_t line);

		// single operator
		int _sinop(instruction_t line);

		// member operator
		int _member(instruction_t);

		/* memory manage functions */

		// globaly address a pointer (32 bit)
		native_ptr address(pointer_t ptr);

		// address segment:offset (16bit:16bit)
		unit_ptr address_2d(pointer_t ptr);

		// read address data
		unit_t read_addr(pointer_t ptr);

		// assign address with data
		void write_addr(pointer_t ptr, unit_t data);


		unit_t read_addr_pair(pointer_t seg, unit_t offset);

		void write_addr_pair(pointer_t seg, unit_t offset, unit_t data);

		// address within an object
		unit_ptr obj_offset(obj_ptr obj, unit_t offset);

		// push a number into stack
		void push_stack(unit_t data);

		// pop a number from stack
		unit_t pop_stack();

		MemoryPool heap;
		ConstMemoryPool constants;
		Stack stack;

		pointer_t stack_ptr; // %esp
		pointer_t frame_ptr; // %ebp
		pointer_t pc;   // high 8 bit - segment; low 8 bit - offset
	};
}

