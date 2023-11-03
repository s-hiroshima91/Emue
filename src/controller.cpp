#include "controller.hpp"
#include "widget.hpp"
#include <iostream>

Controller::Controller(SDL_Renderer *renderer, SDL_Window *window){
	gameWindow = window;
	button[0] = new Button("R", renderer, buttonSize * 3, buttonSize * 2);
	button[1] = new Button("L", renderer, buttonSize * 3, buttonSize * 2);
	button[2] = new Button(" U ", renderer, buttonSize * 2, buttonSize * 3);
	button[3] = new Button(" D ", renderer, buttonSize * 2, buttonSize * 3);
	button[4] = new Button("A", renderer, buttonSize * 4, buttonSize * 4);
	button[5] = new Button("B", renderer, buttonSize * 4, buttonSize * 4);
	button[6] = new Button("SEL", renderer, buttonSize * 3, buttonSize);
	button[7] = new Button("STR", renderer, buttonSize * 3, buttonSize);
	
	exit = new Button("EXIT", renderer, (winX * magni) >> 2, (winX * magni) >> 2);

	saveANDexit = new Button("SAVE & EXIT", renderer, (winX * magni) >> 2, (winX * magni) >> 2);

	save = new Button("SAVE", renderer, (winX * magni) >> 2, (winX * magni) >> 2);

	rst = new Button("" , renderer, winX * magni, (winY - 16) * magni);
	
	int setting[8] = {7, 6, 4, 5, 0, 1, 2, 3};

	for (int i = 0; i < 8; ++i){
		fingerID[i].key = 1 << setting[i];
		fingerID[i].id = 0xff;
	}

}

void Controller::Drow(SDL_Renderer *renderer){
	
	button[0]->Drow(renderer, posX + buttonSize * 5, posY + winY * magni + buttonSize * 4);
	button[1]->Drow(renderer, posX, posY + winY * magni + buttonSize * 4);
	button[2]->Drow(renderer, posX + buttonSize * 3, posY + winY * magni + buttonSize);
	button[3]->Drow(renderer, posX + buttonSize * 3, posY + winY * magni + buttonSize * 6);
	button[4]->Drow(renderer, posX + winX * magni - buttonSize * 4, posY + winY * magni + buttonSize * 3);
	button[5]->Drow(renderer, posX + winX * magni - buttonSize * 9, posY + winY * magni + buttonSize * 3);
	button[6]->Drow(renderer, posX + (winX * magni >> 1) - buttonSize * 3, posY + winY * magni + buttonSize * 9);
	button[7]->Drow(renderer, posX + (winX * magni >> 1) + buttonSize, posY + winY * magni + buttonSize * 9);
	
	exit->Drow(renderer, posX + (winX * magni * 3 >> 2), posY + (winY * magni * 7 >> 2));
	saveANDexit->Drow(renderer, posX + (winX * magni * 3 >> 3), posY + (winY * magni * 7 >> 2));

	save->Drow(renderer, posX, posY + (winY * magni * 7 >> 2));
	
	rst->Drow(renderer, posX, posY);
}

void Controller::Click(int *hardFlg, uint8_t *key){
	int i, x, y;

	SDL_Point point = {0,0};
	
	SDL_GetWindowSize(gameWindow, &x, &y);
	
	while( SDL_PollEvent(&event)){
		
		if (event.type == SDL_FINGERDOWN){
			point.x = static_cast<int>(event.tfinger.x * x);
			point.y = static_cast<int>(event.tfinger.y * y);
			
			i = 0;
			while (i < 8){
				if(button[i]->OnClick(&point))
				{
					fingerID[i].id = event.tfinger.fingerId;
					*key |= fingerID[i].key;
					i = 0x80;
				}else{
					++i;
				}
			}
			if (i != 0x80){
				if (exit->OnClick(&point)){
					*hardFlg = 0x01;
				
				}else if (saveANDexit->OnClick(&point)){
					*hardFlg = 0x03;
					
				}else if (save->OnClick(&point)){
					*hardFlg = 0x02;
				
				}else if (rst->OnClick(&point)){
					*hardFlg = 0x04;
				}
			}
			
		}else if (event.type == SDL_FINGERUP){
			i = 0;

			while (i < 8){
				if (fingerID[i].id == event.tfinger.fingerId){
					fingerID[i].id = 0xff;
					*key &= (fingerID[i].key ^ 0xff);
					i |= 0x08;
				}else{
					++i;
				}
			}
		}
		
	}
	
}

Controller::~Controller(){
	
	delete button[0];
	delete button[1];
	delete button[2];
	delete button[3];
	delete button[4];
	delete button[5];
	delete button[6];
	delete button[7];
	
	delete exit;
	delete saveANDexit;
	delete save;
	delete rst;
	
}