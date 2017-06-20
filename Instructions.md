# CalcVM Instruction Set and Built-in Functions

## Introduction
An instruction is 4 bytes long, devided into three parts:
- Code: 1 byte long
  - low 4 bit: Instruction type;
  - High 4 bit: Sub-instruction type;
- Memory code: 1 byte long
  - Low 4 bit: Memory source;
  - High 4 bit: Data width;
- Data: 2 bytes long, can be data or pointer;

## Memory Codes

### Memory source
- i: From immediate number;
- a: From immediate address;
- m: From memory address;
- s: From stack;

### Data width
- u: 4 bytes long int;
- f: 8 bytes long, float;

## Codes

### hlt
[0x0] Simply stop the VM. Will release resources.

### mov
[0x1] Move data into specified address. Destination address is stored in stack top, but data can from instruction, memory address or stack top;

- imovu: move from immediate number;
- amovu: move from immediate address;
- mmovu: move from another address stored in stack top;
- smovu: move from stack top to address;

### push

[0x2] Push data to stack top. Source can be from immedate number or address.

### pop

[0x3] Pop data from stack top to specified address.

- apopu: Pop to immediate address;
- spopu: Pop to shift of frame pointer;

### Bin operators

[0x4] All begin with 0x4, but with different subcodes.

#### add

#### sub

#### mul

#### div

#### and

#### or

#### xor

### Single Operators
[0x5] 

#### pass
[0x0] Do nothing. 

#### inc
[0x1] Increment by one.

#### dec
[0x2] Decrement by one.

#### not
[0x3] Perform bitwise not;

### Member-Get Operators
[0x6]

#### get

#### set

### jmp
[0xa] Jump to a specified address.

### call
[0xb] Call a function by pointer stored in data or stack top.

### ret
[0xc] Return from a function call. Restore pc, stack pointer and frame pointer.

