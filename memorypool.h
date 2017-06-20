#pragma once

/*
CVM Memory address routine:

REVERVED ADDRESS: 0x0 ~ 0xf, regiesters;
CONSTANT POOL: 0x0f ~ 0x10000, stored as objects;
HEAP: 0x1000 ~ 0xff00 0000, stored as objects;
STACK: 0xff00 0000 ~ 0xffff ffff, stored as unit_t;
*/

#define MEMORY_EXPANSION	10
#define STACK_EXPANSION		2

namespace cvm {

	// allocate an object
	obj_ptr alloc_obj(unit_t size);

	// free an object
	void free_obj(obj_ptr);

	class CalcVM;

	class MemoryPool {
	public:
		MemoryPool(CalcVM*);
		~MemoryPool();

		pointer_t allocate(unit_t size);
		
			// address within the pool
		obj_ptr address(pointer_t ptr);
		
		void free(pointer_t ptr);
		
		void dispose();
	private:
		struct Entry {
			pointer_t ptr;
			enum Flag {
				EMPTY, FULL
			} flag;

			obj_ptr obj;
		};

		Entry* _address(pointer_t ptr);
		
		void check_expansion();
		
		unit_t capacity;
		
		unit_t size;
		
		Entry* pool; // can be implemented by unordered_set, look_up array
		
		CalcVM* host;
	};

	class ConstMemoryPool {
	public:
		ConstMemoryPool(CalcVM*);
		~ConstMemoryPool();

		// allocate a fixed size
		void allocate(size_t size_);

		// address an object
		obj_ptr address(pointer_t ptr);

		void dispose();
	private:
		obj_ptr pool;
		size_t size;
		CalcVM* host;
	};

		// unit_t based stack implementation
	class Stack{
	public:
		Stack(CalcVM*);
		~Stack();

		unit_ptr address(pointer_t offset); // safe indexor
		
		void dispose();

	private:
		
		void expand(pointer_t demand_addr);

		pointer_t size;
		unit_ptr pool; // can be implemented by vector
		CalcVM* host;
	};
}