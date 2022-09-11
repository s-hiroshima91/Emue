/*IOPortのヘッダー*/

#ifndef IOPORT_H
#define IOPORT_H

class Ppu;

class IOPort{


public:
int ioFlg;
bool padFlg;
bool temp;
char padStatus = 0xff;
char padTemp = 0;
bool dma;

unsigned short writeAddr;
char ppuIO[0x0008] = {};
char padIO[0x0020] = {};
Ppu *ppuClass;


IOPort();
void IOFlg(unsigned short addr);
void IOFunc();
void PadFlg(unsigned short addr);
void PadFunc();
void DMAFunc(char *addr);

};

#endif