#include "game.hpp"
#include "cartridge.hpp"
#include "mapper.hpp"
#include "cpu.hpp"
#include "widget.hpp"
#include "ppu.hpp"
#include "ioport.hpp"
#include "controller.hpp"
#include "common.hpp"
#include "color.hpp"
#include <iostream>

void Game::Dummy(){
	
}

Game::Game(std::string romName, SDL_Renderer *render, SDL_Window *window)
{
	renderer = render;
	cartridge = new Cartridge(romName);
	mapper = new Mapper(cartridge);
	ppu = new Ppu(mapper);
	A12 = &Ppu::Ppu::A12;
	controller = new Controller(renderer, window);
	ioPort = new IOPort(ppu, &key);
	cpu = new Cpu(cartridge->romFeature, mapper, ioPort);
	
	label = new Label(romName, renderer, winX * magni, posY >> 1);
	
	wholeTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, dispX, dispY);
	
	if (wholeTexture == NULL)
	{
		std::cout<< "SDL_CreateTexture Error" << std::endl;
		return;
	}
	SDL_SetRenderTarget(renderer, wholeTexture);
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	
	label->Drow(renderer, posX, posY >> 2);
	controller->Drow(renderer);
	SDL_SetRenderTarget(renderer, NULL);
	
	SDL_Delay(500);
	SDL_Event event;
	while( SDL_PollEvent(&event)){
	}

}

void Game::Play(){
		std::chrono::high_resolution_clock::time_point start, end;
	
	int quitFlg = 0;
	int timeStep = 0;
	
	SDL_Rect wholeRect = {0, 0, dispX, dispY};

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, winX, winY - 16);
	
	if (texture == NULL)
	{
		std::cout<< "SDL_CreateTexture Error" << std::endl;
		return;
	}

	Drown = &Game::Drow;
	
	while(quitFlg == 0){
		start = std::chrono::high_resolution_clock::now();
		controller->Click(&quitFlg, &key);

		if (timeStep != 0){
			Drown = &Game::Dummy;
			Sequence();
			Drown = &Game::Drow;

		}else{
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, wholeTexture, NULL, &wholeRect);
			Sequence();
			SDL_RenderPresent(renderer);
		}

		if (quitFlg != 0){
			if ((quitFlg & 0x04) != 0){
				cpu->irq.irqStr.rst = true;
			}
			if ((quitFlg & 0x02) != 0){
				if (cartridge->romFeature->flg.battery){
					cartridge->WriteSaveDate();
				}
			}
	
			quitFlg &= 0x01;				
		/*	if (cpu->ioReg[0x50] != 0){
				cartridge->WriteSaveDate();
			}*/
			
			/*for (quitFlg = 0; quitFlg < 0x100; ++quitFlg){
				for (int test = 0; test <0x10; ++test){
			std::cout << +*ppu->MemoryMap(0x2000 + (quitFlg << 4) + test) << " ";
				}
				std::cout<<std::endl;
			}*/
		}

		end = std::chrono::high_resolution_clock::now();
		auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		timeStep += static_cast<int>(microsec);
		//std::cout << std::dec << timeStep << std::endl;
		if (timeStep < 16666){
			timeStep = (16666 - timeStep) / 1000;
			SDL_Delay(timeStep);
			timeStep = 0;
		}else{
			timeStep -= 16666;
			timeStep &= 0xfffff;
		}
	}
}

void Game::Drow(){
	int num;
		
	for (int j = 0; j < winX; ++j){
		num = static_cast<int>(ppu->ppuPalette[img[j]]);
		pixels[pixelCounter++] = color[num];
	}
}

void Game::Sequence(){

	SDL_LockTexture(texture, NULL, &pixelptr, &pixelCounter);
	pixelCounter = 0;
	pixels = static_cast<int32_t*>(pixelptr);
	
	/*Pre Renderingの処理*/
	while(counter < 2){
		counter +=  cpu->mainRun();
	}

	ppu->ppuStatus &= 0b00011111;
	ppu->s0Flg = -1;

	while (counter < 258){
		counter += cpu->mainRun();
	}

	if((ppu->ppuMask & 0x18) != 0){
		ppu->a12 = (ppu->*(ppu->*A12))(static_cast<uint16_t>(ppu->ppuCtr & 0x10) << (12 - 4));

		ppu->addr.v &= 0b0000010000011111;
		ppu->addr.v |= (ppu->addr.t & 0b0111101111100000);
	}
		
	while(counter < 280){//280 - 258
		counter += cpu->mainRun();
	}

	if((ppu->ppuMask & 0x18) != 0){
		ppu->addr.v &= 0b0111101111100000;
		ppu->addr.v |= (ppu->addr.t & 0b0000010000011111);
	}
		
	while(counter < 305){//304 - 281
		counter += cpu->mainRun();
	}

	if((ppu->ppuMask & 0x18) != 0){
		if (CheckBit(ppu->ppuCtr, 5)){
			ppu->a12 = (ppu->*(ppu->*A12))(0x1000);
		}else{
			ppu->a12 = (ppu->*(ppu->*A12))(static_cast<uint16_t>(ppu->ppuCtr & 0x08) << (12 - 3));
		}
		
		ppu->addr.v &= 0b0111101111100000;
		ppu->addr.v |= (ppu->addr.t & 0b0000010000011111);
	}
	
	while(counter < 341){//340 - 305
		counter += cpu->mainRun();
	}
	counter -= 341;
	ppu->spriteNum = 0;
	ppu->lineCounter = 0;

	/*0ライン目から239ライン目の処理*/
	for (int i = 0; i < winY; ++i){
		/*1ライン以上のppuの時間が経過するまでcpuを動かす*/

		while (counter < 258){//257 - 0
			counter += cpu->mainRun();
		}

		if((ppu->ppuMask & 0x18) != 0){
			ppu->CreateImg(img);
			if ((ppu->lineCounter >= 8) && (ppu->lineCounter < (winY -8))){
				(this->*Drown)();
			}
		}
		
		while(counter < 321){
			counter += cpu->mainRun();
		}

		if ((ppu->ppuMask & 0x18) != 0){
			ppu->SpriteImg();
		}
	
		while(counter < 341){//340 - 258
			counter += cpu->mainRun();
		}
		counter -= 341;
		++ppu->lineCounter;
	}
		
	/*post rendering*/
	while(counter < 341 + 2){
		counter += cpu->mainRun();
	}
	counter -= 341;
		
	//vblank
	*ioPort->ioReg[2] |= 0x80;
		
	//vblank割り込み
	if ((*ioPort->ioReg[0] & 0x80) == 0x80){
		cpu->irq.irqStr.nmi = true;
	}
		
	/*241ライン目から260ライン目の処理*/
	while (counter < 341 * 20){
		counter += cpu->mainRun();
	}
	counter -= 341 * 20;
		
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(renderer, texture, NULL, &rect);

}

Game::~Game(){
	delete cartridge;
	delete cpu;
	delete ppu;
	delete ioPort;
	delete controller;
	delete mapper;
	delete label;
}