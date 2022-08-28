#include "readrom.h"

#define KB 1024
#define headerSize 16

#include <iostream>
#include <fstream>
#include <string>


ReadRom::ReadRom(SDL_Renderer *renderer){
	long int length;
	char filePath[32] = "./rom/";
FileName(filePath, renderer);
	std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
	if (!ifs){
		std::cout << "ファイルが開けませんでした。" << std::endl;

	}
	romHeader = static_cast<char*>(calloc(headerSize, sizeof(char)));
	ifs.read(romHeader, headerSize);

	ifs.seekg(headerSize);
	length = static_cast<int>(romHeader[4] * 16 * KB);
	prgRom = static_cast<char*>(calloc(length,  sizeof(char)));
	ifs.read(prgRom, length);
	
	if (romHeader[5] == 0){
		chrRom = static_cast<char*>(calloc(0x2000, sizeof(char)));
	}else{
		ifs.seekg(headerSize + length);
		length = static_cast<int>(romHeader[5] * 8 * KB);
		chrRom = static_cast<char*>(calloc(length, sizeof(char)));
		ifs.read(chrRom, length);
	}
	ifs.close();
}

void ReadRom::FileName(char *filePath, SDL_Renderer *renderer){
/*	SDL_Window* window;
	SDL_Renderer* renderer;*/
	SDL_Texture* texture;
	SDL_Surface* surface;
	SDL_Event event;
	
	char fileName[32] = "";
	bool inputFlg = true;
	int length = 0;
	
	SDL_Rect letter;
	letter.y = 100;
	letter.h = 40;


	if (TTF_Init() == -1){
		std::cout << "TTF_Init Error" << std::endl;
		return;
	}
	TTF_Font *font = TTF_OpenFont("/system/fonts/Roboto-Regular.ttf", 256);
	if (font == NULL)
	{
		std::cout << "TTF_OpenFont Error" << std::endl;
		return;
	}
	
	SDL_Color color = {0x88, 0x88, 0x88};
SDL_StartTextInput();
	while (inputFlg){
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_MOUSEBUTTONDOWN:
                    /* 終了 */
                    inputFlg = false;
                    
                    	SDL_StopTextInput();
                	break;
                case SDL_TEXTINPUT:
                    /* テキストの末尾に新しいテキストを追加する */
                    	strcat(fileName, event.text.text);
						length += 1;
                    break;
                case SDL_TEXTEDITING:
                    /*
                    未変換テキストを更新する.
                    カーソル位置を更新する.
                    選択の長さを変換する(可能ならば).
                    */
//                  str = event.edit.text;
                    break;
            }
        }
        
        letter.w = length * 40;
        letter.x = 540 - length * 20; 
        
        surface = TTF_RenderText_Solid(font, fileName, color);
        texture =  SDL_CreateTextureFromSurface(renderer, surface);
        
    SDL_FreeSurface(surface);
/*	if (texture == NULL)
	{
		std::cout << stderr << "SDL_CreateTextureFromSurface Error" << std::endl;
		return;
	}*/
    SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, &letter);
	SDL_RenderPresent(renderer);
	}
	
	strcat(filePath, fileName);
	strcat(filePath, ".nes");

}

	

ReadRom::~ReadRom(){
	free(romHeader);
	free(prgRom);
	free(chrRom);
}
