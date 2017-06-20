#include "cvm.h"
#include <cstring>

namespace cvm {

	obj_ptr alloc_obj(unit_t size) {
		obj_ptr obj = new Object;
		obj->size = size;
		obj->val = new unit_t[size];
		obj->type = TYPE_VAR;
		return obj;
	}

	void free_obj(obj_ptr obj) {
		delete[] obj->val;
		obj->size = 0;
		obj->type = TYPE_UNKNOWN;
	}

	ConstMemoryPool::ConstMemoryPool(CalcVM* host_) :
		size(0),
		pool(nullptr),
		host(host_) {

	}

	ConstMemoryPool::~ConstMemoryPool() {
		dispose();
	}

	void ConstMemoryPool::allocate(size_t size_) {
		if (size != 0) {
			host->raise(ERR_MODIFY_CONST);
		}
		if (size > CONST_LIMIT) {
			host->raise(ERR_MEM_OVER_LIMIT);
		}
		size = size_;
		pool = new Object[size_];
	}

	obj_ptr ConstMemoryPool::address(pointer_t ptr) {
		if (ptr >= size) {
			host->ret = ptr;
			host->raise(ERR_CONST_ADDR_EXCESS);
		}
		return pool + ptr;
	}

	void ConstMemoryPool::dispose() {
		for (size_t i = 0; i < size; i++) {
			free_obj(pool + i);
		}
		delete[] pool;
		pool = nullptr;
		size = 0;
	}

	MemoryPool::MemoryPool(CalcVM* host_) :
		capacity(0),
		size(0),
		pool(nullptr),
		host(host_)
	{

	}

	MemoryPool::~MemoryPool() {
		delete[] pool;
	}

	pointer_t MemoryPool::allocate(unit_t size_) {
		check_expansion();

		if (size == HEAP_LIMIT) {
			host->raise(ERR_MEM_OVER_LIMIT);
		}

		for (pointer_t i = 0; i < size; i++) {
			if (pool[i].flag == Entry::EMPTY) {
				pool[i].obj = alloc_obj(size_);
				return i;
			}
		}

		pool[size].obj = alloc_obj(size_);
		pool[size].ptr = size;

		return size++;
	}

	obj_ptr MemoryPool::address(pointer_t ptr) {
		obj_ptr obj = _address(ptr)->obj;
		if (!obj) {
			host->raise(ERR_PAGE_EMPTY);
		}
		return obj;
	}

	void MemoryPool::free(pointer_t ptr) {
		Entry* p = _address(ptr);
		free_obj(p->obj);
		p->flag = Entry::EMPTY;
	}

	void MemoryPool::dispose() {
		for (size_t i = 0; i < size; i++) {
			if (pool[i].flag != Entry::EMPTY) {
				free_obj(pool[i].obj);
			}
		}
		delete[] pool;
		pool = nullptr;
	}

	MemoryPool::Entry* MemoryPool::_address(pointer_t ptr)
	{
		if (ptr >= size) {
			host->ret = ptr;
			host->raise(ERR_PAGE_EXCESS);
		}

		for (Entry* pool_ptr = pool; pool_ptr < pool + ptr; pool_ptr++) {
			if (pool_ptr->flag == Entry::EMPTY)continue;

			if (pool_ptr->ptr == ptr) {
				return pool_ptr;
			}
		}
		host->ret = ptr;
		host->raise(ERR_PAGE_NOT_FOUND);
		return nullptr;
	}

	void MemoryPool::check_expansion() {
		if (size == capacity) {
			Entry* old_pool = pool;
			pool = new Entry[capacity += MEMORY_EXPANSION];
			memcpy(pool, old_pool, sizeof(size));
			delete[] old_pool;
		}
	}


	Stack::Stack(CalcVM* host_) :
		size(256),
		pool(new unit_t[size]),
		host(host_) {

	}
	Stack::~Stack() {
		dispose();
	}

	unit_ptr Stack::address(pointer_t offset)
	{
		if (offset > STACK_LIMIT) {
			host->ret = offset;
			host->raise(ERR_STACK_OVERFLOW);
			return nullptr;
		}
		if (offset >= size) {
			expand(offset);
		}

		return pool + offset;
	}

	void Stack::dispose()
	{
		delete[] pool;
		pool = nullptr;
		size = 0;
	}

	void Stack::expand(pointer_t demand_addr)
	{
		pointer_t new_size = size;
		while (demand_addr >= new_size) {
			new_size *= 2;
		}
		unit_ptr old_pool = pool;
		pool = new unit_t[new_size];
		memcpy(pool, old_pool, size);
		delete[] old_pool;
		size = new_size;
	}

}