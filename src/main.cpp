#include "mips.h"
#include <iostream>

int main(int argc, char * argv[]) {
	if(argc != 2) {
		std::cout << "Invalid or No Arguments found!" << std::endl;
		std::cout << "Usage:" << argv[0] << " prog.bin" << std::endl;
		exit(1);
	}
	mips mips{1024};
	mips.load(argv[1]);
	mips.execute();
	mips.display();
	return 0;
}