# CalcVM Instruction Set and Built-in Functions

## Introduction data structure
An instruction is 4 bytes long, devided into three parts:
- Code: 1 byte long
  - low 4 bit: Instruction type;
  - High 4 bit: Sub-instruction type;
- Memory code: 1 byte long
  - Low 4 bit: Memory source;
  - High 4 bit: Data width;
- Data: 2 bytes long, can be data or pointer;

## Memory Codes

- Immediate number: For example, $1, $a, will be translated as immediate number;
- Offset: Just plain number/symbol, will be translated as memory offset;
- Immediate address: Symbol inside bracket, for example, (1), (a), will be translated into direct address;
- Register address: Only %esp (stack ptr), %ebp (frame ptr), %pc (programe counter), %ret (return register) is valid.

## Assembly Codes

Ending with 'u' means unit data type (pointer/int), 'f' means float data type.

### hlt
[0x0] Simply stop the VM. Will release resources.

### mov
[0x1] Move data into specified address. Destination address is stored in stack top, but data can from instruction, memory address or stack top;

### copy

[0x2] Move data from one address to another.

### push

[0x3] Push data to stack top. Source can be from immedate number or address.

### pop

[0x4] Pop data from stack top to specified address.

### Bin operators

[0x5] Result is written into stack top;

#### add
[0x05]

#### sub
[0x15]

#### mul
[0x25]

#### div
[0x35]

### mod
[0x45]

#### and
[0x55]

#### or
[0x65]

#### xor
[0x75]

### Single Operators
[0x6] 

#### pass
[0x06] Do nothing. 

#### inc
[0x16] Increment by one.

#### dec
[0x26] Decrement by one.

#### not
[0x36] Perform bitwise not;

### Member-Get Operators
[0x7]

#### get
[0x07]

#### set
[0x17]

### Jump

[0xa] Jump to a specified address.

#### jmp
[0x0a]

#### je
[0x1a] Jump when stack top equals zero;

#### jne
[0x2a] Jump when stack top does not equal zero;

#### jg
[0x3a] Jump when stack top is greater than zero;

#### jge
[0x4a] Jump when stack top is greater or equal zero;

### call
[0xb] Call a function by pointer stored in data or stack top.

### Return
[0xc] 

#### leave
[0x0c] Return from a function call. Restore stack pointer and frame pointer.

#### ret
[0x1c] Return from a function call. Restore stack pointer and frame pointer and pc.

### int
[0xd] Interrupt the system. Arguemts should be placed in stack top.
