/*ppuのヘッダー*/

#ifndef PPU_H
#define PPU_H

struct sprite{
	int counter = -1;
	int x = 0;
	bool bgFlg;
	char pattern[16][8] = {};
};

struct Addr{
	unsigned short t = 0;
	unsigned short v = 0;
	unsigned short x = 0;
	bool w = false;
};

struct Screen{
	char* a;
	char* b;
	char* c;
	char* d;
};

class IOPort;

class Ppu{

/*void CreateImg();
void DrowImg();*/

public:
	char ctrRegister1;
	char ctrRegister2;
	bool vMirror;
	bool hMirror;
	int name = 0;


	int lineCounter = 0;
	void CreateImg(char *ptr);
	unsigned short PrePattern(unsigned short vAddr, bool flg);
	char PreConvColor(unsigned short vAddr);
	void SpriteImg8(int x, int y);
	void SpriteImg16(int x, int y);

	Ppu(char *romDate, char header6, IOPort *ioP);
//	char Sequence(char counter);

	char ppuTable[0x1000] = {};
	char ppuPalette[0x20] ={};
	char spriteTable[0x100] = {};
	char *rom1, *rom2, *rom3, *rom4, *rom5, *rom6, *rom7, *rom8;
	struct sprite spriteBuffer[8];
	int bufferNum = 0;
	
	char *sprite0Pattern;
	int *sprite0X;
	int sprite0Y = -1;
	
	struct Addr addr;
	
	struct Screen screen;

	char *ppuIO;
	char *padIO;
	IOPort *ioPort;
	char *MemoryMap(unsigned short addr);

};

#endif