#include "mips.h"
#include <iostream>
#include <fstream>
#include <bitset>
#include <arpa/inet.h>

static const uint insShift = 26;
static const uint rsShift = 21;
static const uint rtShift = 16;
static const uint rdShift = 11;
static const uint shmShift = 6;
static const uint8_t linkReg = 31;
static const uint8_t vReg = 2;
static const uint8_t aReg = 4;
static const uint32_t immMask = ((1 << 16) - 1);
static const uint32_t rsMask = ((1 << 5) - 1) << rsShift;
static const uint32_t rtMask = ((1 << 5) - 1) << rtShift;
static const uint32_t rdMask = ((1 << 5) - 1) << rdShift;
static const uint32_t shmMask = ((1 << 5) - 1) << shmShift;
static const uint32_t funMask = ((1 << 6) - 1);
static const uint32_t adrMask = ((1 << 26) - 1);

mips::mips(uint32_t programSize) : 
	pc{0},
	programSize{programSize},
	exitStatus{false} {
	memory = new uint32_t[programSize];
}

void mips::load(char * file) {
	std::ifstream ifs;
	ifs.open(file, std::ifstream::in | std::ifstream::binary);
	uint32_t loadAddr = 0;
	while(ifs.good() && loadAddr < programSize) {
		uint32_t src;
		ifs.read(reinterpret_cast<char *>(&src), sizeof(uint32_t));
		memory[loadAddr] = htonl(src);
		loadAddr++;
	};
}

void mips::execute() {
	while(!exitStatus && pc <= programSize) {
		registers[0] = 0; //$zero is always 0
		op(memory[pc]);
	}
}

void mips::op(uint32_t op) {
	uint8_t instruction = op >> insShift;
	switch(instruction) {
		case 0x00: { //R-Type
			uint8_t rs = (op & rsMask) >> rsShift;
			uint8_t rt = (op & rtMask) >> rtShift;
			uint8_t rd = (op & rdMask) >> rdShift;
			uint8_t shm = (op & shmMask) >> shmShift;
			uint8_t fun = op & funMask;
			r(instruction, registers[rs], registers[rt], registers[rd], shm, fun);
			break;
		}
		case 0x02: { //JUMP
			uint32_t adr = op & adrMask;
			pc = (pc & 0xF0000000) | adr;
			break;
		}
		case 0x03: {
			registers[linkReg] = pc + 1;
			uint32_t adr = op & adrMask;
			pc = (pc & 0xF0000000) | adr;
			break;
		}
		case 0x08: { //ADDI
			uint8_t rs = (op & rsMask) >> rsShift;
			uint8_t rt = (op & rtMask) >> rtShift;
			int32_t imm = op & immMask;
			registers[rs] = (int32_t) registers[rt] + imm;
			pc++;
			break;
		}
		case 0x09: { //ADDIU
			uint8_t rs = (op & rsMask) >> rsShift;
			uint8_t rt = (op & rtMask) >> rtShift;
			uint32_t imm = op & immMask;
			registers[rs] = registers[rt] + imm;
			pc++;
			break;
		}
		case 0x0C: { //ANDI
			uint8_t rs = (op & rsMask) >> rsShift;
			uint8_t rt = (op & rtMask) >> rtShift;
			uint32_t imm = op & immMask;
			registers[rt] = registers[rs] & imm;
			pc++;
			break;
		}
		default: {
			std::cout << "Unsupported or Invalid Instruction:" 
				<< pc << ":" << std::bitset<32>{op} << std::endl;
			pc++;
		}
	}
}

void mips::r(uint8_t instruction, uint32_t & rs, uint32_t & rt, uint32_t & rd, uint8_t shm, uint8_t fun) {
	switch(fun) {
		case 0x00: { //SLL
			rd = rt << shm;
			pc++;
			break;
		}
		case 0x02: { //SRL
			rd = rt >> shm;
			pc++;
			break;
		}
		case 0x03: { //SRA
			rd = ((int32_t) rt) >> shm;
			pc++;
			break;
		}
		case 0x04: { //SLLV
			rd = rt << rs;
			pc++;
			break;
		}
		case 0x06: { //SRLV
			rd = rt >> rs;
			pc++;
			break;
		}
		case 0x07: { //SRAV
			rd = ((int32_t) rt) >> rs;
			pc++;
			break;
		}
		case 0x08: { //JR
			pc = rs;
			pc++;
			break;
		}
		case 0x0C: { //System Call
			switch(registers[vReg]) {
				case 0x01: {
					std::cout << registers[aReg] << std::endl;
					break;
				}
				case 0x05: {
					std::cin >> registers[vReg];			
					break;
				}
				case 0x0A: {
					std::cout << "Exit Signal Recieved!" << std::endl;
					exitStatus = true;				
					break;
				}
				default: {
					std::cout << "Invalid System Call:" << pc << ":" << registers[vReg] << std::endl;
					break;
				}
			}
			pc++;
			break;
		}
		case 0x20: { //ADD
			rs = (int32_t) rt + (int32_t) rd;
			pc++;
			break;
		}
		case 0x21: { //ADDU
			rs = rt + rd;
			pc++;
			break;
		}
		case 0x22: { //SUB
			rs = (int32_t) rt - (int32_t) rd;
			pc++;
			break;
		}
		case 0x23: { //SUBU
			rs = rt - rd;
			pc++;
			break;
		}
		case 0x24: { //AND
			rs = rt & rd;
			pc++;
			break;
		}
		case 0x25: { //OR
			rs = rt | rd;
			pc++;
			break;
		}
		case 0x26: { //XOR
			rs = rt ^ rd;
			pc++;
			break;
		}
		case 0x27: { //NOR
			rs = (rt ^ (0u - 1u)) & (rd ^ (0u - 1u));
			pc++;
			break;
		}
		default: {
			std::cout << "Invalid R-Type Instruction:" << std::bitset<8>(fun) << std::endl;
			pc++;
			break;
		}
	}
}

mips::~mips() {
	delete memory;
}

void mips::display() const {
	std::cout << "pc: " << pc << std::endl;
	for(unsigned int i = 0; i < 32; i++) {
		std::cout << "r" << i << ": " << registers[i] << std::endl;	
	}
}
	