#ifndef READROM_H
#define READROM_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

class ReadRom {
public:
	char *romHeader, *prgRom, *chrRom, *extRam;
	char savPath[32] = "./save/";
	

	ReadRom(SDL_Renderer *renderer);
	~ReadRom();
	void FileName(char *romPath, SDL_Renderer *renderer);
	void SaveDate(int mapper);
};

#endif