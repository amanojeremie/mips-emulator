#ifndef MIPS_H
#define MIPS_H
#include <cstdint>
class mips {
	public:
		mips(uint32_t programSize);
		~mips();
		void execute();
		void load(char * file);
		void display() const;
	private:
		void op(uint32_t op);
		void r(uint8_t instruction, uint32_t & rs, uint32_t & rt, uint32_t & rd, uint8_t shm, uint8_t fun);
		uint32_t * memory;
		uint32_t pc;
		uint32_t programSize;
		bool exitStatus;
		uint32_t registers[32];
		uint32_t hi;
		uint32_t lo;
};
#endif