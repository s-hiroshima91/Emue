/*mapperのソース*/

#include "mapper.h"
#include "ppu.h"
#include "common.h"
#include <iostream>

Mapper::Mapper(char header6, Ppu *Ppu){
	ppuClass = Ppu;	
	mapper = C2I(header6) >> 4;

}

char *Mapper::BankSelect(char *prgRom1, char bankNum){
	
	if (mapper == 1){
		return prgRom1;
	}else if (mapper == 2){
		return Mapper002(prgRom1, bankNum);

	}else if (mapper == 3){
		Mapper003(bankNum);
		return prgRom1;
	}
	
}

char *Mapper::Mapper002(char *prgRom1,char bankNum){
	int selectBank;
	selectBank = (C2I(bankNum) - prgBank) << 14;
	prgBank = C2I(bankNum);
	prgRom1 += selectBank * sizeof(char);
	return prgRom1;
}


void Mapper::Mapper003(char bankNum){
	int selectBank;
	selectBank = (C2I(bankNum) - chrBank) << 13;
	chrBank = C2I(bankNum);
	ppuClass->rom += selectBank * sizeof(char);
}
