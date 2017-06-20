#include "cvm.h"
#include "assemble.h"

using namespace cvm;


int main() {
	CInput input;

    readfile("test.calca", input);

	CalcVM calc;
	calc.load(&input);
	return calc.run();
}