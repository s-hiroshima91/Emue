/*ppuのヘッダー*/

#ifndef PPU_H
#define PPU_H

struct sprite{
	int counter = -1;
	int x = 0;
	bool bgFlg;
	char pattern[16][8] = {};
};

class IOPort;

class Ppu{

/*void CreateImg();
void DrowImg();*/

public:
	char ctrRegister1;
	char ctrRegister2;
	bool vMirror;

	char scroll[2] = {};
	int lineCounter = 0;
	void CreateImg(char *ptr);
	int PrePattern(int x, int y, int name, int line, bool flg);
	char PreConvColor(int x, int y, int name, int line);
	void SpriteImg8(int x, int y);
	void SpriteImg16(int x, int y);

	Ppu(char *romDate, char header6, IOPort *ioP);
//	char Sequence(char counter);

	char ppuTable[0x1000] = {};
	char ppuPalette[0x20] ={};
	char spriteTable[0x100] = {};
	char *rom;
	struct sprite spriteBuffer[8];
	int bufferNum = 0;

	char *ppuIO;
	char *padIO;
	IOPort *ioPort;
	char *MemoryMap(unsigned short addr);

};

#endif