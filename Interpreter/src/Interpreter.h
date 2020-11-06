#pragma once
#include <array>

#define NNN_MASK 0x0FFF
#define KK_MASK  0x00FF
#define X_MASK   0x0F00
#define Y_MASK   0x00F0
#define N_MASK   0x000F

class Interpreter
{
private:
	std::array<unsigned char, 4096> m_memory;
	std::array<unsigned char, 16> m_v_reg;
	unsigned short m_i;
	unsigned char m_dt;
	unsigned short m_pc;
	unsigned char m_stack_ptr;
	std::array<unsigned short, 16> m_stack;

	unsigned short m_opcode;

public:
	std::array<unsigned char, 2048> buffer;
	std::array<bool, 16> key_buffer;
	unsigned char st;

	bool should_draw;

public:
	Interpreter();
	~Interpreter();

	void load_rom(char* src, int length);
	void cycle();
};