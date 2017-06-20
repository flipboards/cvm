#pragma once

#ifndef CVM_MEMORY
#define CVM_MEMORY


#include "memorypool.h"

#define CONST_BEGIN		0x00000010
#define CONST_END		0x00010000
#define CONST_LIMIT		0x0000fff0
#define HEAP_BEGIN		0x00010000
#define HEAP_END		0xff000000
#define HEAP_LIMIT		0xfeff0000
#define STACK_BEGIN		0xff000000
#define STACK_END		0xffffffff
#define STACK_LIMIT		0x00ffffff

#define ADDR_STACK_PTR	0x1
#define ADDR_FRAME_PTR	0x2
#define ADDR_PC			0x3
#define ADDR_RESULT		0xa



#endif // !CVM_MEMORY
