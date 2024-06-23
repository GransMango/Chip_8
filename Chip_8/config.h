#pragma once

#define CHIP_8_MEMORY_SIZE 4096
#define CHIP_8_REGISTER_SIZE 16
#define CHIP_8_STACK_SIZE 16
#define CHIP_8_GFX_SIZE 64 * 32
#define CHIP_8_KEYPAD_SIZE 16

#define CHIP_8_FONTSET_SIZE 80
#define CHIP_8_FONT_WIDTH 5
#define CHIP_8_FONTSET_START_ADDRESS 0x50

#define CHIP_8_WIDTH 64
#define CHIP_8_HEIGHT 32

#define CHIP_8_START_ADDRESS 0x200


// Fetched from https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
extern unsigned char fontset[CHIP_8_FONTSET_SIZE];