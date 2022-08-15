#ifndef READROM_H
#define READROM_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

class ReadRom {
public:
	char *romHeader, *prgRom, *chrRom;
	

	ReadRom(SDL_Renderer *renderer);
	~ReadRom();
	void FileName(char *filename, SDL_Renderer *renderer);
};

#endif