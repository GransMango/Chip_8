#include "chip_8.hpp"
#include "graphics.hpp"
#include <iostream>

int main(int argc, char** argv) {
    /*
    if (argc < 2) {
        std::cerr << "Usage: chip8 <filename>" << std::endl;
        return 1;
    }
    */
    Chip8 chip8;
    chip8.loadGame("H:\\downloads\\Other\\flightrunner.ch8");

    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    while (true) {
        frameStart = SDL_GetTicks();

        chip8.emulateCycle();
        chip8.graphics.setKeys(chip8);
        chip8.graphics.drawScreen(chip8);

        frameTime = SDL_GetTicks() - frameStart;

        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    return 0;
}