/* Implementation of cvm instrucions */

#include "calcvm.h"
#include "error.h"

using namespace cvm;

int CalcVM::_movu(instruction_t line)
{
	switch (line.mem_code)
	{
	case CVM_INSTRUCT_MEM_IMDT: // *m <= d           movu $data
		write_addr(read_addr(stack_ptr), (unit_t)line.data);
		break;
	case CVM_INSTRUCT_MEM_ADDR: { // *(m1 + d) <= m2  movu data
		pointer_t dst = pop_stack();
		write_addr(dst += (int)line.data, read_addr(stack_ptr));
		break;
	}
	case CVM_INSTRUCT_MEM_LOCAL: { // *(m1 + d) <= *m2 bmovu data
		pointer_t dst = pop_stack();
		write_addr(dst += (int)line.data, read_addr(read_addr(stack_ptr)));
		break;
	}
	case CVM_INSTRUCT_MEM_REG: // *m <= *d           movu (data) / movu %ret
		write_addr(read_addr(stack_ptr), read_addr((unit_t)line.data));
		break;
	default:
		raise(ERR_INVALID_INSTRUCT);
		return 1;
	}
	return 0;
}

int CalcVM::_copyu(instruction_t line) {
	switch (line.mem_code)
	{
	case CVM_INSTRUCT_MEM_OFFSET: // *(s + d) <= *m
		write_addr(frame_ptr + (int)line.data, read_addr(stack_ptr));
		break;
	case CVM_INSTRUCT_MEM_LOCAL: { // *m2 <= *(m1 + d)
		pointer_t src = pop_stack();
		write_addr(read_addr(stack_ptr), read_addr(src + (int)line.data));
		break;
	}
	case CVM_INSTRUCT_MEM_REG: // *d <= *m
		write_addr((unit_t)line.data, read_addr(stack_ptr));
		break;
	default:
		raise(ERR_INVALID_INSTRUCT);
		break;
		return 1;
	}
	return 0;
}

int CalcVM::_pushu(instruction_t line)
{
	switch (line.mem_code)
	{
	case CVM_INSTRUCT_MEM_IMDT: // m <= d          pushu $data
		push_stack((unit_t)line.data);
		break;
	case CVM_INSTRUCT_MEM_ADDR: // m <= *(m + d)   pushu data(%esp)
		push_stack(read_addr(pop_stack() + (int)line.data));
		break;
	case CVM_INSTRUCT_MEM_OFFSET: // m <= *(s + d) pushu data(%ebp)
		push_stack(read_addr(frame_ptr + (int)line.data));
		break;
	case CVM_INSTRUCT_MEM_REG: // m <= *d          pushu ($data) / pushu %ret
		push_stack(read_addr((unit_t)line.data));
		break;
	default:
		raise(ERR_INVALID_INSTRUCT);
		return 1;
	}
	return 0;
}

int CalcVM::_popu(instruction_t line) {

	unit_t data = pop_stack();
	switch (line.mem_code)
	{
    case CVM_INSTRUCT_MEM_IMDT:
        break;
	case CVM_INSTRUCT_MEM_ADDR:  // *(m2 + d) <= m1
		write_addr(read_addr(stack_ptr) + (int)line.data, data);
		break;
	case CVM_INSTRUCT_MEM_OFFSET: // *(s + d) <= m  popu data(%ebp)
		write_addr(frame_ptr + (int)line.data, data);
		break;
	case CVM_INSTRUCT_MEM_REG: // *d <= m           popu ($data) / popu %ret
		write_addr((unit_t)line.data, data);
		break;
	default:
		raise(ERR_INVALID_INSTRUCT);
		return 1;
	}
	return 0;
}

int CalcVM::_jmpu(instruction_t line)
{
	pointer_t dst;
	switch (line.mem_code)
	{
	case CVM_INSTRUCT_MEM_IMDT: //    
		dst = (unit_t)line.data << SEG_SIZE;
		break;
	case CVM_INSTRUCT_MEM_ADDR: //   jmpu data(%esp)
		dst = pop_stack() + (int)line.data;
		break;
	case CVM_INSTRUCT_MEM_OFFSET: // jmpu $data
		dst = (pc & 0xffff0000) + line.data;
		break;
	default:
		raise(ERR_INVALID_INSTRUCT);
		return 1;
	}

	switch (line.code >> 4)
	{
	case CVM_JMP_ALWAYS:
		pc = dst; break;
	case CVM_JMP_ZERO:
		if (read_addr(stack_ptr) == 0)pc = dst;
		break;
	case CVM_JMP_NONZERO:
		if (read_addr(stack_ptr) != 0)pc = dst;
		break;
	case CVM_JMP_POSITIVE:
		if ((int)read_addr(stack_ptr) > 0) pc = dst;
		break;
	case CVM_JMP_NONNEG:
		if ((int)read_addr(stack_ptr) >= 0) pc = dst;
		break;
	default:
		raise(ERR_INVALID_INSTRUCT);
		break;
	}

	return 0;
}

int CalcVM::_call(instruction_t line)
{
	push_stack(pc);
	push_stack(frame_ptr);
	frame_ptr = stack_ptr;

	switch (line.mem_code)
	{
	case CVM_INSTRUCT_MEM_IMDT: //
		return call((unit_t)line.data);
	case CVM_INSTRUCT_MEM_ADDR: //   call data(%esp)
		return call(pop_stack() + (int)line.data);
	case CVM_INSTRUCT_MEM_OFFSET: // call $data
		pc = (pc & 0xffff0000) + line.data;
		return 0;
	default:
		raise(ERR_INVALID_INSTRUCT);
		return 1;
	}
	return 0;
}

int CalcVM::_ret(instruction_t line) {
	stack_ptr = frame_ptr;
	frame_ptr = pop_stack();

    if (line.code >> 4 == 1) {
	    pc = pop_stack();
    }

	return 0;
}

int CalcVM::_binop(instruction_t line)
{
	unit_t rhs;
	switch (line.mem_code)
	{
	case CVM_INSTRUCT_MEM_IMDT:  // binu $data
		rhs = (unit_t)line.data;
		break;
	case CVM_INSTRUCT_MEM_LOCAL: // binu (%esp)
		rhs = pop_stack();
		break;
	default:
		raise(ERR_INVALID_INSTRUCT);
		return 1;
	}
	unit_t lhs = read_addr(stack_ptr);

	switch (line.code >> INSTRUCT_TYPECODE_LENGTH)
	{
	case CVM_BINOP_ADD:
		write_addr(stack_ptr, lhs + rhs); break;
	case CVM_BINOP_SUB:
		write_addr(stack_ptr, lhs - rhs); break;
	case CVM_BINOP_MUL:
		write_addr(stack_ptr, lhs * rhs); break;
	case CVM_BINOP_DIV:
		write_addr(stack_ptr, lhs / rhs); break;
	case CVM_BINOP_MOD:
		write_addr(stack_ptr, lhs % rhs); break;
	case CVM_BINOP_AND:
		write_addr(stack_ptr, lhs & rhs); break;
	case CVM_BINOP_OR:
		write_addr(stack_ptr, lhs | rhs); break;
	case CVM_BINOP_XOR:
		write_addr(stack_ptr, lhs ^ rhs); break;
	default:
		raise(ERR_INVALID_INSTRUCT);
		return 1;
	}
	return 0;
}

int CalcVM::_sinop(instruction_t line)
{
	switch (line.code >> 4)
	{
	case CVM_SINOP_PASS:
		break;
	case CVM_SINOP_ADDONE:
		write_addr(stack_ptr, read_addr(stack_ptr) + 1);
		break;
	case CVM_SINOP_SUBONE:
		write_addr(stack_ptr, read_addr(stack_ptr) - 1);
		break;
	case CVM_SINOP_NOT:
		write_addr(stack_ptr, !read_addr(stack_ptr));
		break;
	default:
		raise(ERR_INVALID_INSTRUCT);
		return 1;
	}
	return 0;
}

int CalcVM::_member(instruction_t line) {
	switch (line.code >> INSTRUCT_TYPECODE_LENGTH)
	{
	case CVM_MBR_GETMB: {
		unit_t offset = pop_stack();
		write_addr(stack_ptr, read_addr_pair(read_addr(stack_ptr), offset));
		break;
	}
	case CVM_MBR_SETMB: {
		unit_t offset = pop_stack();
		unit_t data = pop_stack();
		write_addr_pair(read_addr(stack_ptr), offset, data);
		break;
	}
	default:
		raise(ERR_INVALID_INSTRUCT);
		return 1;
	}
	return 0;
}