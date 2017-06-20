#include "cvm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace cvm {

	CalcVM::CalcVM() :
		heap(this),
		constants(this),
		stack(this),
		frame_ptr(0),
		pc(0)
	{

	}

	CalcVM::~CalcVM() {
		release();
	}

	int CalcVM::load(CInput* input) {
		//count number

		constants.allocate(input->staticvar_num + input->nativefunc_num + input->function_num);

		unit_t const_ptr = 0;

		for (int i = 0; i < input->nativefunc_num; i++) {
			obj_ptr const_obj = constants.address(const_ptr);
			const_obj->type = TYPE_NATIVE_FUNC;
			const_obj->size = sizeof(NativeFunc) / sizeof(unit_t);
			const_obj->val = new unit_t[const_obj->size];
			memcpy(const_obj->val, &input->nativefuncs[i], const_obj->size * sizeof(unit_t));
			const_ptr++;
		}
		for (int i = 0; i < input->staticvar_num; i++) {
			obj_ptr const_obj = constants.address(const_ptr);
			const_obj->type = TYPE_VAR;
			const_obj->size = input->staticvars[i].size;
			const_obj->val = new unit_t[const_obj->size];
			memcpy(const_obj->val, input->staticvars[i].val, const_obj->size * sizeof(unit_t));
			const_ptr++;
		}

		for (int i = 0; i < input->function_num; i++) {
			obj_ptr const_obj = constants.address(const_ptr);
			const_obj->type = TYPE_FUNC;
			const_obj->size = input->functions[i].size * sizeof(instruction_t) / sizeof(unit_t);
			const_obj->val = new unit_t[const_obj->size];
			memcpy(const_obj->val, input->functions[i].instructions, const_obj->size * sizeof(unit_t));
			const_ptr++;
		}

		pc = input->head.entry;
		stack_ptr = STACK_BEGIN;
		frame_ptr = STACK_BEGIN;
		return 0;
	}

	int CalcVM::run() {
		while (!status) {
			try {
				execute();
			}
			catch (RuntimeError) {
				check_error();
				return 1;
			}
		}
		return 0;
	}

	int CalcVM::execute() {
		//fetch
		instruction_t line = *(instruction_t*)address_2d(pc);
		pc++;

		switch (line.code & 0xf)
		{
		case CVM_INSTRUCT_HLT:
			interrupt(INT_EXIT);
			return 0;
		case CVM_INSTRUCT_MOV:
			return _movu(line);
		case CVM_INSTRUCT_COPY:
			return _copyu(line);
		case CVM_INSTRUCT_PUSH:
			return _pushu(line);
		case CVM_INSTRUCT_POP:
			return _popu(line);
		case CVM_INSTRUCT_BINOP:
			return _binop(line);
		case CVM_INSTRUCT_SINOP:
			return _sinop(line);
		case CVM_INSTRUCT_MBR:
			return _member(line);
		case CVM_INSTRUCT_JMP:
			return _jmpu(line);
		case CVM_INSTRUCT_CALL:
			return _call(line);
		case CVM_INSTRUCT_RET:
			return _ret(line);
		case CVM_INSTRUCT_INT:
			interrupt(line.data);
			return 0;
		default:
			raise(ERR_INVALID_INSTRUCT);
			return 1;
		}

	}


	int CalcVM::call(pointer_t func_ptr) {
		obj_ptr func = (obj_ptr)address(func_ptr);

		switch (func->type)
		{
		case TYPE_FUNC:
			pc = func_ptr << SEG_SIZE;	//[func:0x0000]
			return 0;

		case TYPE_NATIVE_FUNC:
		{
			if (!func->val || func->size != sizeof(NativeFunc)) {
				ret = func_ptr;
				raise(ERR_OBJECT_UNCALLABLE);
				return 1;
			}

			NativeFunc* native_func = (NativeFunc*)func->val;

			native_ptr* argv = new native_ptr[native_func->argc];
			for (unit_t i = 0; i < native_func->argc; i++) {
				if ((native_func->arg_descriptor >> i) & 1) {
					argv[i] = address(read_addr(frame_ptr - 2 - i));
				}
				else {
					argv[i] = address(frame_ptr - 2 - i);
				}
			}

			if (status = native_func->func_ptr(argv, &ret)) {
				raise(status += ERR_NATIVE_FUNC_BEGIN);
				return 1;
			}
			return 0;
		}

		default:
			ret = func_ptr;
			raise(ERR_OBJECT_UNCALLABLE);
			return 1; // object_not_callable
		}
	}


	native_ptr CalcVM::address(pointer_t ptr) {

		// special address
		if (ptr < CONST_BEGIN) {
			switch (ptr)
			{
			case ADDR_STACK_PTR:
				return &stack_ptr;
			case ADDR_FRAME_PTR:
				return &frame_ptr;
			case ADDR_PC:
				return &pc;
			case ADDR_RESULT:
				return &ret;
			default:
				ret = ptr;
				raise(ERR_INVALID_ADDR);
				return nullptr;
			}
		}
		// constants
		else if (ptr < CONST_END) {
			return constants.address(ptr - CONST_BEGIN);
		}
		// heap
		else if (ptr < HEAP_END) {
			return heap.address(ptr - HEAP_BEGIN);
		}
		// stack
		else if (ptr < STACK_END) {
			return stack.address(ptr - STACK_BEGIN);
		}
		// won't actually happen
		else {
			ret = ptr;
			raise(ERR_INVALID_ADDR);
			return nullptr;
		}
	}

	unit_ptr CalcVM::address_2d(pointer_t ptr)
	{
		return obj_offset((obj_ptr)address(ptr >> SEG_SIZE), ptr & SEG_SIZE_MASK);
	}

	unit_t CalcVM::read_addr(pointer_t ptr)
	{
		return *(unit_ptr)address(ptr);
	}

	void CalcVM::write_addr(pointer_t ptr, unit_t data)
	{
		*(unit_ptr)address(ptr) = data;
	}

	unit_t CalcVM::read_addr_pair(pointer_t seg, unit_t offset)
	{
		return *obj_offset((obj_ptr)address(seg), offset);
	}

	void CalcVM::write_addr_pair(pointer_t seg, unit_t offset, unit_t data)
	{
		*obj_offset((obj_ptr)address(seg), offset) = data;
	}

	unit_ptr CalcVM::obj_offset(obj_ptr obj, unit_t offset)
	{
		if (offset < obj->size)
			return obj->val + offset;
		else {
			ret = offset;
			raise(ERR_INVALID_OFFSET);
			return nullptr;
		}
	}

	void CalcVM::push_stack(unit_t data)
	{
		write_addr(++stack_ptr, data);
	}

	unit_t CalcVM::pop_stack()
	{
		return read_addr(stack_ptr--);
	}


	void CalcVM::interrupt(unit_t int_no)
	{
		switch (int_no)
		{
		case INT_ALLOC: // alloc [size]
			heap.allocate(pop_stack() - HEAP_BEGIN);
			break;
		case INT_FREE: {  // free would be unnecessary if GC works
			pointer_t ptr = pop_stack();
			if (ptr != 0) {
				heap.free(ptr - HEAP_BEGIN);
			}
			break;
		}
		case INT_PRINT:
			printf("%d\n", (int)pop_stack());
			break;
		case INT_EXIT:
			status = ERR_TERMINATION;
			release();
			break;
		case INT_ERROR:
			throw RuntimeError();
			break;
		default:
			ret = int_no;
			raise(ERR_INVALID_INTRRUPT);
			break;
		}
	}

	void CalcVM::raise(unit_t error)
	{
		status = error;
		interrupt(INT_ERROR);
	}

	void CalcVM::check_error()
	{
		printf("Error at %d (%d-%d):\n", pc, pc >> SEG_SIZE, pc & SEG_SIZE_MASK);
		if (status > ERR_NATIVE_FUNC_BEGIN) {
			printf("Native function error with code %d\n", status);
			return;
		}

		switch (status)
		{
		case ERR_INVALID_INSTRUCT:
			printf("Invalid instruction\n");
			break;
		case ERR_INVALID_ADDR:
			printf("Invalid address for %d\n", ret);
			break;
		case ERR_INVALID_INTRRUPT:
			printf("Invalid interrupt number %d\n", ret);
			break;
		case ERR_ENTRY_MISSING:
			printf("No main entry\n");
			break;
		case ERR_ENTRY_UNCERTAIN:
			printf("More than one main entry\n");
			break;
		case ERR_OBJECT_UNCALLABLE:
			printf("Object at %d is not callable\n", ret);
			break;
		case ERR_INVALID_OFFSET:
			printf("Offset %d is invalid\n", ret);
			break;
		case ERR_PAGE_NOT_FOUND:
			printf("Page not found for %d\n", ret);
			break;
		case ERR_PAGE_EXCESS:
			printf("Page excess for %d\n", ret);
			break;
		case ERR_CONST_ADDR_EXCESS:
			printf("Const address excess for %d\n", ret);
			break;
		case ERR_STACK_EXCESS:
			printf("Stack excess for %d\n", ret);
			break;
		case ERR_STACK_OVERFLOW:
			printf("Stack overflow\n");
			break;
		case ERR_STACK_EMPTY:
			printf("Pop from empty stack\n");
			break;
		default:
			printf("Unknown Exception\n");
			break;
		}
	}

	int CalcVM::release()
	{
		stack.dispose();
		constants.dispose();
		heap.dispose();
		return 0;
	}


}