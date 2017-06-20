#pragma once

/*
Definition of CVM instruction;

*/

#ifndef CVM_INSTRUCT
#define CVM_INSTRUCT

// Global instruction table

#define CVM_INSTRUCT_HLT	0x0
#define CVM_INSTRUCT_MOV	0x1
#define CVM_INSTRUCT_COPY	0x2
#define CVM_INSTRUCT_PUSH	0x3
#define CVM_INSTRUCT_POP	0x4
#define CVM_INSTRUCT_BINOP	0x5
#define CVM_INSTRUCT_SINOP	0x6
#define CVM_INSTRUCT_MBR	0x7
#define CVM_INSTRUCT_JMP	0xa
#define CVM_INSTRUCT_CALL	0xb
#define CVM_INSTRUCT_RET	0xc
#define CVM_INSTRUCT_INT	0xd

/* Binary operator */

#define CVM_BINOP_ADD		0x0
#define CVM_BINOP_SUB		0x1
#define CVM_BINOP_MUL		0x2
#define CVM_BINOP_DIV		0x3
#define CVM_BINOP_MOD		0x4
#define CVM_BINOP_AND		0x5
#define CVM_BINOP_OR		0x6
#define CVM_BINOP_XOR		0x7

/* Single operator */

#define CVM_SINOP_PASS		0x0
#define CVM_SINOP_ADDONE	0x1
#define CVM_SINOP_SUBONE	0x2
#define CVM_SINOP_NOT		0x3

/* Get member operator */

#define CVM_MBR_GETMB		0x0
#define CVM_MBR_SETMB		0x1

/* Condition jump */
#define CVM_JMP_ALWAYS		0x0
#define CVM_JMP_ZERO		0x1 // =0
#define CVM_JMP_NONZERO		0x2 // !=0
#define CVM_JMP_POSITIVE	0x3 // >0
#define CVM_JMP_NONNEG		0x4	// >=0

/* Ret/Leave */

#define CVM_RET_LEAVE       0x0 // leave
#define CVM_RET_PC          0x1 // ret

/* Memory flag */

#define CVM_INSTRUCT_MEM_IMDT	0x00 //  <- d
#define CVM_INSTRUCT_MEM_ADDR	0x01 //  <- m
#define CVM_INSTRUCT_MEM_OFFSET	0x02 //  <- s
#define CVM_INSTRUCT_MEM_LOCAL	0x03 //  <- *m
#define CVM_INSTRUCT_MEM_REG	0x07 //  <- *d

// CVM Interrupt Table

#define INT_ALLOC		0x1
#define INT_FREE		0x2
#define INT_PRINT		0xa
#define INT_EXIT		0xe
#define INT_ERROR		0xf


#endif // !CVM_INSTRUCT
