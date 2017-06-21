# CalcVM Architecture

## Primitive Data types

__unit__: 4 bytes, unsigned int;
__float__: 8 bytes, float (unsupport now);

## Object

A special data type which can only created in heap/constant region. The whole object has only one address and instruction _get_ and _set_ plus offset for r/w inside the region. Each object has a fixed memory size determined by creation.

## Memory architecture

In default, cvm memory uses two ways for addressing:

__Direct address__: Memory address is mapped to a real address in contignuous region;
__Slab address__: Memory address is mapped to an object (with a fixed memory size);

### Stack

A contiguous memory segments using direct address, and the default working memory. Keeps information with call stack and local variables. Use vector-like implementation and dynamically increases size (which is transparent in addressing).

### Constants 

Implemented by an array of objects using slab address, which is determined when loading.

### Heap

Using slab address, and can insert/delete object by interrupt. Currently implemented by an array with (addr, obbject), and is addressed by searching the address. But can also be implemented by hash map.

### Addressing

By default, address range of each memory segment:
- Constants: 0x08 ~ 0x10000;
- Heap: 0x1000 ~ 0xff00 0000;
- Stack: 0xff00 0000 ~ 0xffff ffff;

The following address are reserved as special address:
- stack_ptr: 0x01
- frame_ptr: 0x02
- pc: 0x03
- ret: 0x10

0 is reserved as NULL.

