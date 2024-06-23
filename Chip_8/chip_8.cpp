#include "chip_8.hpp"
#include <iostream>
#include <fstream>
#include <string>

Chip8::Chip8() {
	pc = CHIP_8_START_ADDRESS;
	opcode = 0;
	I = 0; 
	sp = 0; 
	delay_timer = 0; 
	sound_timer = 0;
	drawFlag = false;


	// Clear display
	for (int i = 0; i < CHIP_8_GFX_SIZE; i++) {
		gfx[i] = 0;
	}

	// Clear stack
	for (int i = 0; i < CHIP_8_STACK_SIZE; i++) {
		stack[i] = 0;
	}

	// Clear registers V0-VF
	for (int i = 0; i < CHIP_8_REGISTER_SIZE; i++) {
		V[i] = 0;
	}

	for (int i = 0; i < CHIP_8_MEMORY_SIZE; i++) {
		memory[i] = 0;
	}

	// Load fontset
	for (int i = 0; i < CHIP_8_FONTSET_SIZE; i++) {
		memory[CHIP_8_FONTSET_START_ADDRESS + i] = fontset[i];
	}
	
	for (int i = 0; i < CHIP_8_KEYPAD_SIZE; i++) {
		keys[i] = 0;
	}
}

void Chip8::loadGame(const char* filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		exit(1);
	}

	file.seekg(0, std::ios::end);
	std::streampos size = file.tellg();
	file.seekg(0, std::ios::beg);
	
	if (size > CHIP_8_MEMORY_SIZE - CHIP_8_START_ADDRESS) {
		std::cerr << "File too large: " << filename << std::endl;
		exit(1);
	}

	file.read((char*)&memory[CHIP_8_START_ADDRESS], size);
	file.close();

	std::cout << "Loaded: " << filename << std::endl;
}

void Chip8::emulateCycle() {
	// opcode is 2 bytes
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Decode opcode
	switch (opcode & 0xF000) {
	case 0x0000:
		switch (opcode & 0x00FF) {
		case 0x00E0:
			clearScreen();
			break;
		case 0x00EE:
			returnFromSubroutine();
			break;
		default:
			// 0x0NNN, not implemented as it is not used
			increaseOpcode();
			break;
		}
		break;
	case 0x1000:
		jumpToAddress();
		break;
	case 0x2000:
		callSubroutine();
		break;
	case 0x3000:
		skipIfEqual();
		break;
	case 0x4000:
		skipIfNotEqual();
		break;
	case 0x5000:
		skipIfRegisterEqual();
		break;
	case 0x6000:
		setRegister();
		break;
	case 0x7000:
		addRegister();
		break;
	case 0x8000:
		switch (opcode & 0x000F) {
		case 0x0000:
			setRegisterToRegister();
			break;
		case 0x0001:
			orRegister();
			break;
		case 0x0002:
			andRegister();
			break;
		case 0x0003:
			xorRegister();
			break;
		case 0x0004:
			addRegister();
			break;
		case 0x0005:
			subtractRegisterCheckVF();
			break;
		case 0x0006:
			shiftRight();
			break;
		case 0x0007:
			subtractRegisterCheckVFReverse();
			break;
		case 0x000E:
			shiftLeft();
			break;
		}
		break;
	case 0x9000:
		skipIfNotEqualRegister();
		break;
	case 0xA000:
		setI();
		break;
	case 0xB000:
		jumpToAddressPlusV0();
		break;
	case 0xC000:
		random();
		break;
	case 0xD000:
		draw();
		break;
	case 0xE000:
		switch (opcode & 0x00FF) {
		case 0x009E:
			skipIfKeyPressed();
			break;
		case 0x00A1:
			skipIfKeyNotPressed();
			break;
		}
	case 0xF000:
		switch (opcode & 0x00FF) {
		case 0x0007:
			setRegisterToDT();
			break;
		case 0x000A:
			waitForKeyPress();
			break;
		case 0x0015:
			setDelayTimerToRegister();
			break;
		case 0x0018:
			setSoundTimerToRegister();
			break;
		case 0x001E:
			addRegisterToI();
			break;
		case 0x0029:
			setIToFont();
			break;
		case 0x0033:
			storeBCD();
			break;
		case 0x0055:
			storeRegisters();
			break;
		case 0x0065:
			loadRegisters();
			break;
		}
	default:
		std::cerr << "Unknown opcode: " << opcode << std::endl;
		printf("Opcode was %x\n", opcode);
		exit(1);
		break;
	}
}

void Chip8::increaseOpcode() {
	pc += 2;
}

void Chip8::clearScreen() {
	for (int i = 0; i < CHIP_8_GFX_SIZE; i++) {
		gfx[i] = 0;
	}
	drawFlag = true;
	increaseOpcode();
}

void Chip8::returnFromSubroutine() {
	pc = stack[sp];
	sp--;
	if (sp < 0) {
		std::cerr << "Stack underflow" << std::endl;
		exit(2);
	}
}

void Chip8::jumpToAddress() {
	pc = opcode & 0x0FFF;
}

void Chip8::callSubroutine() {
	sp++;
	if (sp >= CHIP_8_STACK_SIZE) {
		std::cerr << "Stack overflow" << std::endl;
		exit(1);
	}
	stack[sp] = pc;
	pc = opcode & 0x0FFF;
}

void Chip8::skipIfEqual() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;

	if (V[x] == kk) {
		pc += 2;
	}
	pc += 2;
}

void Chip8::skipIfNotEqual() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;

	if (V[x] != kk) {
		pc += 2;
	}
	pc += 2;
}

void Chip8::skipIfRegisterEqual() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;

	if (V[x] == V[y]) {
		pc += 2;
	}
	pc += 2;
}

void Chip8::setRegister() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;

	V[x] = kk;
	pc += 2;
}

void Chip8::addRegister() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;

	V[x] += kk;
	pc += 2;
}

void Chip8::setRegisterToRegister() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;

	V[x] = V[y];
	pc += 2;
}

void Chip8::orRegister() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;

	V[x] |= V[y];
	pc += 2;
}

void Chip8::andRegister() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;

	V[x] &= V[y];
	pc += 2;
}

void Chip8::xorRegister() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;

	V[x] ^= V[y];
	pc += 2;
}

void Chip8::addRegisterCheckVF() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	uint16_t sum = V[x] + V[y];

	if (sum > 0xFF) {
		V[0xF] = 1;
	}
	else {
		V[0xF] = 0;
	}
	V[x] = sum & 0xFF;
	pc += 2;
}

void Chip8::subtractRegisterCheckVF() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;

	if (V[x] > V[y]) {
		V[0xF] = 1;
	}
	else {
		V[0xF] = 0;
	}
	V[x] -= V[y];
	pc += 2;
}

void Chip8::shiftRight() {
	uint8_t x = (opcode & 0x0F00) >> 8;

	V[0xF] = V[x] & 0x1;
	V[x] >>= 1;
	pc += 2;
}

void Chip8::subtractRegisterCheckVFReverse() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;

	if (V[y] > V[x]) {
		V[0xF] = 1;
	}
	else {
		V[0xF] = 0;
	}
	V[x] = V[y] - V[x];
	pc += 2;
}

void Chip8::shiftLeft() {
	uint8_t x = (opcode & 0x0F00) >> 8;

	V[0xF] = V[x] >> 7;
	V[x] <<= 1;
	pc += 2;
}

void Chip8::skipIfNotEqualRegister() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;

	if (V[x] != V[y]) {
		pc += 2;
	}
	pc += 2;
}

void Chip8::setI() {
	I = opcode & 0x0FFF;
	pc += 2;
}

void Chip8::jumpToAddressPlusV0() {
	pc = (opcode & 0x0FFF) + V[0];
}

void Chip8::random() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;

	V[x] = rand() & kk;
	pc += 2;
}

void Chip8::draw() {
	// DXYN: Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
	// Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
	// The sprite is XORed onto the existing screen
	uint8_t x = V[(opcode & 0x0F00) >> 8];
	uint8_t y = V[(opcode & 0x00F0) >> 4];
	uint8_t memory_read_size = opcode & 0x000F;

	for (int i = 0; i < memory_read_size; i++) {
		uint8_t sprite = memory[I + i];
		for (int j = 0; j < 8; j++) {
			uint8_t pixel = (sprite >> (7 - j)) & 1;
			short screen_x = (x + j) % CHIP_8_WIDTH;
			short screen_y = (y + i) % CHIP_8_HEIGHT;
			short gfx_index = screen_x + screen_y * CHIP_8_WIDTH;
			if (pixel == 1) {
				if (gfx[gfx_index] == 1) {
					V[0xF] = 1;
				}
				gfx[gfx_index] ^= 1;
			}
		}
	}
	drawFlag = true;
	pc += 2;
}

void Chip8::skipIfKeyPressed() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t key = V[x];

	if (keys[key] == 1) {
		pc += 2;
	}
	pc += 2;
}

void Chip8::skipIfKeyNotPressed() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t key = V[x];

	if (keys[key] == 0) {
		pc += 2;
	}
	pc += 2;
}

void Chip8::setRegisterToDT() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	V[x] = delay_timer;
	pc += 2;
}

void Chip8::waitForKeyPress() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	bool key_pressed = false;
	for (int i = 0; i < CHIP_8_KEYPAD_SIZE; i++) {
		if (keys[i] == 1) {
			V[x] = i;
			key_pressed = true;
			break;
		}
	}
	if (!key_pressed) {
		return;
	}
	pc += 2;
}

void Chip8::setDelayTimerToRegister() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	delay_timer = V[x];
	pc += 2;
}

void Chip8::setSoundTimerToRegister() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	sound_timer = V[x];
	pc += 2;
}

void Chip8::addRegisterToI() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	I += V[x];
	pc += 2;
}

void Chip8::setIToFont() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	I = CHIP_8_FONTSET_START_ADDRESS + V[x] * CHIP_8_FONT_WIDTH;
	pc += 2;
}

void Chip8::storeBCD() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t value = V[x];
	memory[I] = value / 100;
	memory[I + 1] = (value / 10) % 10;
	memory[I + 2] = value % 10;
	pc += 2;
}

void Chip8::storeRegisters() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= x; i++) {
		memory[I + i] = V[i];
	}
	pc += 2;
}

void Chip8::loadRegisters() {
	uint8_t x = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= x; i++) {
		V[i] = memory[I + i];
	}
	pc += 2;
}

