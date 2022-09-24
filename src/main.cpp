#include <iostream>
#include <chrono>
#include "readrom.h"
#include "cpu.h"
#include "ioport.h"
#include "ppu.h"
#include "controller.h"
#include "mapper.h"

#include "color.h"

#define dispX 1080
#define dispY 1920
#define posX 100
#define posY 100
#define winX 256
#define winY 240
#define magni 3


int main(int argc, char *argv[])
{
	int quit_flg = 2;
	int x, y;
	int counter = 0;
	int touchX;
	int touchY;
	int temp = 0;
	char padFlg = 0;
	char rstFlg = 0;
	int bg1, bg2, bg3;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event event;
	
	SDL_Rect bgColor, gameWindow;

	bgColor.x =0;
	bgColor.y =0;
	bgColor.w = winX;
	bgColor.h = winY;
	
	gameWindow.x =posX;
	gameWindow.y =posY;
	gameWindow.w = winX * magni;
	gameWindow.h = winY * magni;

	int skipFlg = 0;
	std::chrono::high_resolution_clock::time_point start, end;
	int timeStep;
	
	
	/* 初期化 */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
    	std::cout << "error" << std::endl;
		return 1;
	}
	
	/* ウィンドウ作成 */
	window = SDL_CreateWindow("Nes Emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dispX, dispY, SDL_WINDOW_SHOWN);
	if( window == NULL ) {
		std::cout << "Can not create window" << std::endl;
		return 1;
    }
    
    /*レンダラー呼び出し*/
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (renderer == NULL)
	{
		std::cout<< "SDL_CreateRenderer Error" << std::endl;
		return 1;
	}
	
	/*レンダラー呼び出し*/
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, winX, winY);
	if (texture == NULL)
	{
		std::cout<< "SDL_CreateTexture Error" << std::endl;
		return 1;
	}
	
	/*各クラスの生成*/
	ReadRom RR(renderer);
	IOPort *IOP = new IOPort;
	Ppu *Ppu = new class Ppu(RR.chrRom, RR.romHeader[6], IOP);
	Mapper *Map = new class Mapper(RR.romHeader, Ppu);
	Cpu Cpu(RR.prgRom, RR.romHeader[4], IOP, Map);
	Controller *Controller = new class
	Controller(posX, posY, winX, winY, magni);

	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	Controller->Img(renderer);
	
	/* イベントループ */
	while(quit_flg)
	{

		start = std::chrono::high_resolution_clock::now();
	    
	    SDL_GetWindowSize(window, &x, &y);

		while( SDL_PollEvent(&event) )
		{
			switch (event.type)
			{
				case SDL_FINGERDOWN:
					touchX = static_cast<int>(event.tfinger.x * x);
					touchY = static_cast<int>(event.tfinger.y * y);
					padFlg = Controller->InputDown(touchX, touchY, event.tfinger.fingerId);
					if (padFlg == 0xff){
						Cpu.rst = true;
						padFlg = 0;
					}else if(padFlg == 0xf0){
						quit_flg = 0;
					}else{
						IOP->padTemp |= padFlg;
					}
					break;
				case SDL_FINGERUP:
					IOP->padTemp &= Controller->InputUp(event.tfinger.fingerId);
					break;
				case SDL_QUIT:
					quit_flg = 0;
					break;
			}
        }

        SDL_SetRenderTarget(renderer, texture);

        /*-1ライン目の処理*/
        IOP->ppuIO[0x0002] &= 0b10111111;

/*if((Ppu->ctrRegister2 & 0x18) != 0){
        Ppu->addr.v &= 0b0000010000011111;
        Ppu->addr.v |= (Ppu->addr.t & 0b0111101111100000);
}*/
        while (counter < 341){
			counter += 3 * Cpu.mainRun();
		}
		counter -= 341;
		Cpu.irbr = Map->MapperIRQ(Cpu.irbr);
	
		Ppu->lineCounter = 0;

		if((Ppu->ctrRegister2 & 0x18) != 0){
/*	        Ppu->addr.v &= 0b0000010000011111;
	        Ppu->addr.v |= (Ppu->addr.t & 0b0111101111100000);*/
	        Ppu->addr.v = Ppu->addr.t;
}
		
	
		if(skipFlg == 0){
			SDL_RenderClear(renderer);

			bg1 = static_cast<int>(Ppu->ppuPalette[0]) * 3;
			bg2 = bg1 + 1;
			bg3 = bg1 + 2;

			SDL_SetRenderDrawColor(renderer, color[bg1], color[bg2], color[bg3], SDL_ALPHA_OPAQUE);

			SDL_RenderFillRect(renderer, &bgColor);
		}
    
	    /*0ライン目から239ライン目の処理*/
		for (int i = 0; i < winY; ++i){
			/*1ライン以上のppuの時間が経過するまでcpuを動かす*/
			while (counter < 341){
				counter += 3 * Cpu.mainRun();
			}
			counter -= 341;
			Cpu.irbr = Map->MapperIRQ(Cpu.irbr);

			char bg[winX + 8] = {};
			Ppu->CreateImg(bg);
		
			if (skipFlg == 0){
				for (int j = 0; j < winX; ++j){
					if ((bg[j]& 0b11) !=  0){
	 	   			bg1 = static_cast<int>(Ppu->ppuPalette[bg[j]]) * 3;
		    			bg2 = bg1 + 1;
	 	   			bg3 = bg1 + 2;
	 	   			SDL_SetRenderDrawColor(renderer, color[bg1], color[bg2], color[bg3], SDL_ALPHA_OPAQUE);

						/*■をバッファに描画*/	
								SDL_RenderDrawPoint(renderer, j,  i);
					}
				}
			}
		}

		IOP->ppuIO[0x0002] |= 0b10000000;
		if ((IOP->ppuIO[0x0000] & 0x80) == 0x80){
			Cpu.nmi = 1;

		}
		/*240ライン目から261ライン目の処理*/
		while (counter < 341 * 22){
			counter += 3 * Cpu.mainRun();
		}
		
		SDL_SetRenderTarget(renderer, NULL);
		
		SDL_RenderCopy(renderer, texture, NULL, &gameWindow);

		/*コントローラの描画*/

		SDL_RenderPresent(renderer);

		end = std::chrono::high_resolution_clock::now();
		auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		timeStep += static_cast<int>(millisec);
//		std::cout << timeStep << std::endl;
		if (timeStep <= 16){
	    	SDL_Delay(16 - timeStep);
	    	timeStep = 0;
	    	skipFlg = 0;
		}else if (timeStep <= 48){
			timeStep -= 16;
			skipFlg += 1;
			skipFlg &= 3;
		}else{
			timeStep = 0;
			skipFlg = 0;
		}
	}
	
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
			
			
	SDL_Quit();
	return 0;
}