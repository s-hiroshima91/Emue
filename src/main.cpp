#include <iostream>
#include <fstream>
#include <bitset>
#include <chrono>
//#include "SDL2/SDL.h"
#include "readrom.h"
#include "cpu.h"
#include "ioport.h"
#include "ppu.h"
#include "controller.h"

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
	int touchX;
	int touchY;
	int refuse = 0;
	char padFlg = 0;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;
	
	SDL_Rect bgColor, imgDot;
	imgDot.w = magni;
	imgDot.h = magni;

	bgColor.x = posX;
	bgColor.y = posY;
	bgColor.w = winX * magni;
	bgColor.h = winY * magni;


		std::chrono::high_resolution_clock::time_point start, end;
	
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
	
	ReadRom RR(renderer);
	IOPort *IOP = new IOPort;
	Ppu *Ppu = new class Ppu(RR.chrRom, RR.romHeader[6], IOP);
	Cpu Cpu(RR.prgRom, RR.romHeader[4], IOP);
	Controller *Controller = new class Controller(posX, posY, winX, winY, magni);
	



	/* イベントループ */
	while(quit_flg)
	{
		int counter = 0;

		start = std::chrono::high_resolution_clock::now();
	    
	    SDL_GetWindowSize(window, &x, &y);
	    padFlg = 0;

		while( SDL_PollEvent(&event) )
		{
			switch (event.type)
			{
				case SDL_FINGERDOWN:
				if (refuse == 0)
				{
					touchX = static_cast<int>(event.tfinger.x * x);
					touchY = static_cast<int>(event.tfinger.y * y);
				}
//				quit_flg -= 1;
				IOP->padTemp |= Controller->InputDown(touchX, touchY, event.tfinger.fingerId);
//				Cpu.nmi = 1;
				break;
				case SDL_FINGERUP:
				IOP->padTemp &= Controller->InputUp(event.tfinger.fingerId);
				break;
				case SDL_QUIT:
				quit_flg = 0;
				break;
			}
        }
        
//        quit_flg = 0;

/*	while (counter < 29780){
		counter += Cpu.TestRun();
		if (quit_flg == 0){
//			std::cout << "op:0x_" << std::hex << +Cpu.opeCode << " PC:0x_" << +Cpu.registerPC << " SP:0x_" << std::bitset<8>(Cpu.registerP) << " a:0x_" <<  +Cpu.registerA << " x:0x_" << +Cpu.registerX << " y:0x_" <<+Cpu.registerY << " 0x2006:_0x" << +IOP->writeAddr << " 0x2007:_0x" << +IOP->ppuIO[0x0007] << std::endl;
//std::cout << std::hex << +Cpu.registerPC << " " << +Cpu.registerP << " " << +Cpu.registerS  << std::endl;
		}
	}*/

//	IOP->ppuIO[0x0002] = 0b10000000;
	
/*	for (int i = 0; i < 9000; i++){
		Cpu.TestRun();
	}*/

	while (counter < 341){
		counter += 3 * Cpu.TestRun();
	}
	counter -= 341;
	
	/*レンダラーで背景描画*/
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	Ppu->lineCounter = 0;

	int bg1, bg2, bg3;
	bg1 = static_cast<int>(Ppu->ppuPalette[0]) * 3;
	bg2 = bg1 + 1;
	bg3 = bg1 + 2;
	SDL_SetRenderDrawColor(renderer, color[bg1], color[bg2], color[bg3], SDL_ALPHA_OPAQUE);

	SDL_RenderFillRect(renderer, &bgColor);
	
    imgDot.y = posY;
    
	for (int i = 0; i < winY; i++){
		while (counter < 341){
			counter += 3 * Cpu.TestRun();
		}
		counter -= 341;

		char bg[winX + 8];
		Ppu->CreateImg(bg);

		imgDot.x = posX;
		
		for (int j = 0; j < winX; j++){
			if ((bg[j]& 0b11) !=  0){
    			bg1 = static_cast<int>(Ppu->ppuPalette[bg[j]]) * 3;
    			bg2 = bg1 + 1;
    			bg3 = bg1 + 2;
    			SDL_SetRenderDrawColor(renderer, color[bg1], color[bg2], color[bg3], SDL_ALPHA_OPAQUE);

			/*■をバッファに描画*/
    			SDL_RenderFillRect(renderer, &imgDot);
	
//			SDL_RenderDrawPoint(renderer, j,  i);
			}
			imgDot.x += magni;
		}
		imgDot.y += magni;
	}
	IOP->ppuIO[0x0002] |= 0b10000000;
	if ((IOP->ppuIO[0x0000] & 0x80) == 0x80){
		Cpu.nmi = 1;

	}
	
	while (counter < 341 * 22){
		counter += 3 * Cpu.TestRun();
	}
/*		std::ofstream ofs("test5.txt", std::ios::out | std::ios::binary);
	 if (!ofs)
    {
        std::cout << "ファイルが開けませんでした。" << std::endl;
        return 1;
    }
	ofs.write(Ppu->ppuPallet, sizeof(char) * 0x20);
	ofs.close();*/
	
	Controller->Img(renderer);


	SDL_RenderPresent(renderer);
	end = std::chrono::high_resolution_clock::now();
	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << millisec << std::endl;
	if (millisec < 16){
    	SDL_Delay(16 - millisec);
	}
//	quit_flg = 0;
	}
	
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);

//	SDL_Quit();


	
//	for (int i = 0; i < 0x000; i++){
//	Ppu->CreateImg();
//	}

/*	std::ofstream ofs1("test1.txt", std::ios::out | std::ios::binary);
	 if (!ofs1)
    {
        std::cout << "ファイルが開けませんでした。" << std::endl;
        return 1;
    }
	ofs1.write(Ppu->spriteTable, sizeof(char) * 0x100);
	ofs1.close();

	std::ofstream ofs2("test2.txt", std::ios::out | std::ios::binary);
	 if (!ofs2)
    {
        std::cout << "ファイルが開けませんでした。" << std::endl;
        return 1;
    }
	ofs2.write(Cpu.cpuRam, sizeof(char) * 0x800);
	ofs2.close();	
			
	std::ofstream ofs3("test3.txt", std::ios::out | std::ios::binary);
	 if (!ofs3)
    {
        std::cout << "ファイルが開けませんでした。" << std::endl;
        return 1;
    }
	ofs3.write(Ppu->ppuPallet, sizeof(char) * 0x20);
	ofs3.close();*/
			
			
	SDL_Quit();
	return 0;
}