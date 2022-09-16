/*mapperのヘッダー*/

#ifndef MAPPER_H
#define MAPPER_H

class Ppu;

struct Mp001{
	int counter = 0;
	char sRegister = 0;
	char pBankMode = 3;
	char cBankMode = 0;
	int pBankNum = 0;
	int cBankLowNum = 0;
	int cBankHighNum = 1;
};

class Mapper{


public:

Ppu *ppuClass;
char *header;
int mapper;
char **prgRom1, **prgRom2, **prgRom3, **prgRom4;
char *prgRom, *chrRom;

struct Mp001 mapper001;

Mapper(char *header, Ppu *Ppu);

void CpuRom(char **rom1, char **rom2, char **rom3, char **rom4);

void BankSelect(char bankNum, unsigned short addr);

void PrgLowBank2(int bank16k);
void PrgHighBank2(int bank16k);

void ChrLowBank2(int bank4k);
void ChrHighBank2(int bank4k);

void ChrBank4(int bank8k);

void Mapper001(char bankNum, unsigned short addr);
void Mapper002(char bankNum);
void Mapper003(char bankNum);
void Mapper030(char bankNum);
};

#endif