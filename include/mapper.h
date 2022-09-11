/*mapperのヘッダー*/

#ifndef MAPPER_H
#define MAPPER_H

class Ppu;

class Mapper{


public:

Ppu *ppuClass;
int mapper;
int chrBank = 0;
int prgBank = 0;

Mapper(char header6, Ppu *Ppu);

char *BankSelect(char *prgRom1, char bankNum);

char  *Mapper002(char *prgRom1, char bankNum);
void Mapper003(char bankNum);
};

#endif