#pragma once
#include <cstdint>
#include "config.h"
#include "graphics.hpp"

class Chip8 {
public:
	bool drawFlag;
	uint8_t stack[CHIP_8_STACK_SIZE];
	uint16_t sp;
	uint16_t pc;
	uint16_t opcode;
	uint16_t I; // Point at the memory address
	uint8_t delay_timer;
	uint8_t sound_timer;
	uint8_t memory[CHIP_8_MEMORY_SIZE];
	uint8_t V[CHIP_8_REGISTER_SIZE]; // Registers
	uint8_t gfx[CHIP_8_GFX_SIZE]; // Graphics
	uint8_t keys[CHIP_8_KEYPAD_SIZE]; // Keypad
	Graphics graphics;

	Chip8();
	void emulateCycle();
	void loadGame(const char* filename);
private:
	void clearScreen();
	void returnFromSubroutine();
	void jumpToAddress();
	void callSubroutine();
	void skipIfEqual();
	void skipIfNotEqual();
	void skipIfRegisterEqual();
	void setRegister();
	void addRegister();
	void setRegisterToRegister();
	void orRegister();
	void andRegister();
	void xorRegister();
	void increaseOpcode();
	void addRegisterCheckVF();
	void subtractRegisterCheckVF();
	void shiftRight();
	void subtractRegisterCheckVFReverse();
	void shiftLeft();
	void skipIfNotEqualRegister();
	void setI();
	void jumpToAddressPlusV0();
	void random();
	void draw();
	void skipIfKeyPressed();
	void skipIfKeyNotPressed();
	void setRegisterToDT();
	void waitForKeyPress();
	void setDelayTimerToRegister();
	void setSoundTimerToRegister();
	void addRegisterToI();
	void setIToFont();
	void storeBCD();
	void storeRegisters();
	void loadRegisters();
};