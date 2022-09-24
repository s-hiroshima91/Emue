/*mapperのヘッダー*/

#ifndef MAPPER_H
#define MAPPER_H

class Ppu;

struct mapperStatus{
	int counter = 0;
	char sRegister = 0;
	char pBankMode = 3;
	char cBankMode = 0;
	int bankNum[8] = {0};
//	int cBankLowNum = 0;
//	int cBankHighNum = 1;
};

class Mapper{


public:

Ppu *ppuClass;
char *header;
int mapper;
char **prgRom1, **prgRom2, **prgRom3, **prgRom4;
char *prgRom, *chrRom;
int irqCounter = 0;
int irq;
bool irqEnable;

struct mapperStatus status;

Mapper(char *header, Ppu *Ppu);

void CpuRom(char **rom1, char **rom2, char **rom3, char **rom4);

void BankSelect(char bankNum, unsigned short addr);

void PrgLowBank2(int bank16k);
void PrgHighBank2(int bank16k);

void Chr1stBank2(int bank2k);
void Chr2ndBank2(int bank2k);
void Chr3rdBank2(int bank2k);
void Chr4thBank2(int bank2k);
void ChrLowBank4(int bank4k);
void ChrHighBank4(int bank4k);
void ChrBank8(int bank8k);

bool MapperIRQ(bool irqFlg);

void Mapper001(char bankNum, unsigned short addr);
void Mapper002(char bankNum);
void Mapper003(char bankNum);
void Mapper004(char bankNum, unsigned short addr);
void Mapper030(char bankNum);
};

#endif