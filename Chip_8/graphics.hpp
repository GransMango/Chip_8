#pragma once
#include <SDL.h>

class Chip8;

class Graphics {
public:
	Graphics();
	~Graphics();
	void drawScreen(Chip8& chip8);
	void setKeys(Chip8& chip8);
	void clearScreen();

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
};
