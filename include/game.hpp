#ifndef GAME_HPP
#define GAME_HPP

#include "common.hpp"
#include "SDL2/SDL.h"
#include <string>

class Cartridge;
class Cpu;
class Ppu;
class IOPort;
class Controller;
class Mapper;
class Label;

class Game{
	private:
	void *pixelptr;
	int32_t *pixels;
	int pixelCounter;
	int counter = 0;
	uint8_t key;
	uint8_t img[winX + 8];
	SDL_Rect rect = {posX, posY, winX * magni, (winY - 16) * magni};
	
	Cartridge *cartridge;
	Cpu *cpu;
	Ppu *ppu;
	IOPort *ioPort;
	Controller *controller;
	Mapper *mapper;
	Label *label;
	
	SDL_Texture *texture, *wholeTexture;
	SDL_Renderer *renderer;

	void (Game::*Drown)();
	void Drow();
	void Dummy();
	
	void Sequence();
	
	uint16_t (Ppu::*Ppu::*A12)(uint16_t a12);
	
	public:
	Game(std::string romName, SDL_Renderer *renderer, SDL_Window *window);
	void Play();
	~Game();

};

#endif