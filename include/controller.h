/*コントローラのヘッダー*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "SDL2/SDL.h"

#define btnSize 30

struct fingerID{
	bool status = false;
	SDL_FingerID id;
	char key;
};

class Controller{
public:

	SDL_Rect aBtn, bBtn, selBtn, stBtn, upBtn, downBtn, leftBtn, rightBtn;
	
	struct fingerID fingerID[10];
	
	int pos[6];

	Controller(int posX, int posY, int winX, int winY, int magni);

	char InputDown(int x, int y, SDL_FingerID id);
	char InputUp(SDL_FingerID id);
	void Img(SDL_Renderer* renderer);

};

#endif