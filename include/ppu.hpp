/*ppuのヘッダー*/

#ifndef PPU_HPP
#define PPU_HPP

#include "common.hpp"

class Mapper;

typedef struct {
	int x = 0;
	bool bgFlg;
	uint8_t pattern[8] = {};
} Sprite;

typedef struct {
	uint16_t t = 0;
	uint16_t v = 0;
	uint16_t x = 0;
	bool w = false;
} Addr;

class Ppu{

public:
	uint8_t ppuCtr;
	uint8_t ppuMask;
	uint8_t ppuStatus;
	uint8_t oamAddr;
	uint8_t oamData;
	uint8_t ppuScroll;
	uint8_t ppuAddr;
	uint8_t ppuData;
	uint8_t dmaData;
	int name = 0;


	int lineCounter = 0;
	int s0Flg;
	int spriteSize = 8;
	int spriteNum = 0;
	uint16_t a12;

	uint8_t ppuPalette[0x20] ={};
	uint8_t spriteTable[0x100] = {};
	
	void CreateImg(uint8_t *bg);
	uint16_t PrePattern(uint16_t vAddr, uint16_t flg);
	uint8_t PreConvColor(uint16_t vAddr);
	void SpriteImg();

	Ppu(Mapper *MAPPER);

	Sprite sprite[8];
	
	Addr addr;
	
	Mapper *mapper;

	uint8_t *MemoryMap(uint16_t addr);
	uint16_t Mapper004(uint16_t after);
	uint16_t Dummy(uint16_t after);
	uint16_t (Ppu::*A12)(uint16_t after);

};

#endif