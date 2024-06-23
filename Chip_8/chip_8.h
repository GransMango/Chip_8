#pragma once

class Chip8 {
public:
	bool drawFlag;
	short stack[16];
	short sp;
	short pc;
	short opcode;
	short I; // Point at the memory address
	unsigned char delay_timer;
	unsigned char sound_timer;
	unsigned char memory[4096];
	unsigned char V[16]; // Registers
	unsigned char gfx[64 * 32]; // Graphics

	Chip8();
	~Chip8();

	void emulateCycle();
	void loadGame(const char* filename);
private:
	void setKeys();
};