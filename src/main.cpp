#include "game.hpp"
#include "widget.hpp"
#include "common.hpp"
#include <iostream>
#include <chrono>


void ExitFunc(bool *flg){
	*flg = false;
}

void GameStart(std::string name, SDL_Renderer *renderer, SDL_Window *window){
	
	Game *game;
	game = new Game(name, renderer, window);
	
	game->Play();
	delete game;

}

int main(int argc, char *argv[])
{
	bool quitFlg = true;

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;
	
	/* 初期化 */
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		std::cout << "SDL_Init(): " <<  SDL_GetError() << std::endl;
		return 1;
	}
	/* ウィンドウ作成 */
	window = SDL_CreateWindow("Game Boy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dispX, dispY, SDL_WINDOW_SHOWN);
	if(window == NULL) {
		std::cout << "Can not create window" << std::endl;
		return 1;
	}
	/*レンダラー呼び出し*/
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL){
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	
	TextBox RomName("Input Rom Name", renderer, winX * magni, posY >> 1);
	Button Start("START", renderer, (winX * magni) >> 2, (winX * magni) >> 2);
	Button Exit("EXIT", renderer, (winX * magni) >> 2, (winX * magni) >> 2);
	
	/* イベントループ */
	while(quitFlg){
		while( SDL_PollEvent(&event)){
			Start.OnClick(&event, [&](){GameStart(RomName.name, renderer, window);});
			Exit.OnClick(&event, [&](){ExitFunc(&quitFlg);});
			RomName.OnClick(&event, renderer);
        }
        
		/*レンダラーで背景描画*/
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		
		Start.Drow(renderer, posX, posY + (winY * magni * 7 >> 2));
		Exit.Drow(renderer, posX + (winX * magni * 3 >> 2), posY + (winY * magni * 7 >> 2));
		RomName.Drow(renderer, posX, posY >> 2);
		
		SDL_RenderPresent(renderer);
		SDL_Delay(10);
		
	}
	
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}