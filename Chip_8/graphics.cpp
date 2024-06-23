#include "graphics.hpp"
#include "chip_8.hpp"
#include <iostream>

Graphics::Graphics() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CHIP_8_WIDTH * 10, CHIP_8_HEIGHT * 10, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Could not create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, CHIP_8_WIDTH, CHIP_8_HEIGHT);
    if (!texture) {
        std::cerr << "Could not create texture: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
}

Graphics::~Graphics() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::drawScreen(Chip8& chip8) {
    if (chip8.drawFlag) {
        uint32_t pixels[CHIP_8_WIDTH * CHIP_8_HEIGHT] = { 0 };

        for (int y = 0; y < CHIP_8_HEIGHT; ++y) {
            for (int x = 0; x < CHIP_8_WIDTH; ++x) {
                uint8_t pixel = chip8.gfx[y * CHIP_8_WIDTH + x];
                pixels[y * CHIP_8_WIDTH + x] = (pixel ? 0xFFFFFFFF : 0xFF000000); // White for set pixel, black for unset
            }
        }

        SDL_UpdateTexture(texture, NULL, pixels, CHIP_8_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        chip8.drawFlag = false;
    }
}

void Graphics::clearScreen() {
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void Graphics::setKeys(Chip8& chip8) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(0);
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool isPressed = event.type == SDL_KEYDOWN;
            switch (event.key.keysym.sym) {
            case SDLK_1: chip8.keys[0x1] = isPressed; break;
            case SDLK_2: chip8.keys[0x2] = isPressed; break;
            case SDLK_3: chip8.keys[0x3] = isPressed; break;
            case SDLK_4: chip8.keys[0xC] = isPressed; break;
            case SDLK_q: chip8.keys[0x4] = isPressed; break;
            case SDLK_w: chip8.keys[0x5] = isPressed; break;
            case SDLK_e: chip8.keys[0x6] = isPressed; break;
            case SDLK_r: chip8.keys[0xD] = isPressed; break;
            case SDLK_a: chip8.keys[0x7] = isPressed; break;
            case SDLK_s: chip8.keys[0x8] = isPressed; break;
            case SDLK_d: chip8.keys[0x9] = isPressed; break;
            case SDLK_f: chip8.keys[0xE] = isPressed; break;
            case SDLK_z: chip8.keys[0xA] = isPressed; break;
            case SDLK_x: chip8.keys[0x0] = isPressed; break;
            case SDLK_c: chip8.keys[0xB] = isPressed; break;
            case SDLK_v: chip8.keys[0xF] = isPressed; break;
            }
        }
    }
}
