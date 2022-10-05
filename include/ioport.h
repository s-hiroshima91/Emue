/*IOPortのヘッダー*/

#ifndef IOPORT_H
#define IOPORT_H

class Ppu;

class IOPort{


public:
unsigned short ioFlg;
bool w = false;
bool padFlg;
char padStatus = 0xff;
char padTemp = 0;
bool dma;
bool temp;
bool readFlg;

char ppuIO[0x0008] = {};
char padIO[0x0020] = {};
Ppu *ppuClass;
char readBuffer;


IOPort();
void IOFlg(unsigned short);
void IOFunc(int wFlg);
void PadFlg(unsigned short addr);
void PadFunc();
void DMAFunc(char *addr);

};

#endif