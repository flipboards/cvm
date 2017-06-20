/* CVM Error Table */

#pragma once

#ifndef CVM_ERROR
#define CVM_ERROR

// Processor error [Runtime]

#define ERR_INVALID_INSTRUCT	0x1
#define ERR_INVALID_ADDR		0x2
#define ERR_INVALID_INTRRUPT	0x3
#define ERR_ENTRY_MISSING		0x4
#define ERR_ENTRY_UNCERTAIN		0x5
#define ERR_OBJECT_UNCALLABLE	0x6
#define ERR_INVALID_OFFSET		0x7

#define ERR_TERMINATION			0xf

// Input read error [Code Segment]

#define ERR_INPUT_FILEOPEN		0xe0
#define ERR_INPUT_FILEEND		0xe1
#define ERR_INPUT_EXCESS_BUF	0xe2

// Memory error [Runtime]

#define ERR_PAGE_NOT_FOUND		0xf1
#define ERR_PAGE_EXCESS			0xf2
#define ERR_CONST_ADDR_EXCESS	0xf3
#define ERR_STACK_EXCESS		0xf4
#define ERR_STACK_OVERFLOW		0xf5
#define ERR_STACK_EMPTY			0xf6
#define ERR_PAGE_EMPTY			0xf7
#define ERR_MODIFY_CONST		0xf8
#define ERR_MEM_OVER_LIMIT		0xf9

// Native function error [Runtime]

#define ERR_NATIVE_FUNC_BEGIN	0x100

namespace cvm {

	class RuntimeError {
	public:
		RuntimeError() : _what(nullptr) {

		}
		explicit RuntimeError(const char* str) : _what(str) {

		}
		const char* what()const {
			return _what ? _what : "CalcVM Runtime Exception";
		}
	private:
		const char* _what;
	};

}

#endif // !CVM_ERROR
