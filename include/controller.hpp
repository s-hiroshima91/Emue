#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "common.hpp"
#include "SDL2/SDL.h"
#include <string>

class Button;

typedef struct {
	SDL_FingerID id;
	uint8_t key;
} FingerID;

class Controller{
	private:
	SDL_Event event;
	SDL_Window *gameWindow;
	
	FingerID fingerID[8];
	
	
	public:
	Button *button[8];
	Button *exit, *saveANDexit, *save, *rst;
	
	Controller(SDL_Renderer *renderer, SDL_Window *window);
	void Drow(SDL_Renderer *renerer);
	void Click(int *quitFlg, uint8_t *key);
	~Controller();
	
};


#endif