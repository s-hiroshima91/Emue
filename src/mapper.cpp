/*mapperのソース*/

#include "mapper.h"
#include "ppu.h"
#include "common.h"
#include <iostream>

Mapper::Mapper(char *romHeader, Ppu *Ppu){
	ppuClass = Ppu;
	header = romHeader;
	mapper = C2I(header[6]) >> 4;
	mapper += (header[7] & 0xf0);

}

void Mapper::CpuRom(char **rom1, char **rom2, char **rom3, char **rom4){
	prgRom = *rom1;
	prgRom1 = rom1;
	prgRom2 = rom2;
	prgRom3 = rom3;
	prgRom4 = rom4;
	chrRom = ppuClass->rom1;
	
	if (mapper == 30){
		chrRom =  static_cast<char*>(calloc(32 * 1024, sizeof(char)));
//		ppuClass->rom1 = chrRom;
		ChrBank4(0);
	}
	
}

void Mapper::BankSelect(char bankNum, unsigned short addr){

	if (mapper == 1){
		Mapper001(bankNum, addr);
		
	}else if (mapper == 2){
		Mapper002(bankNum);

	}else if (mapper == 3){
		Mapper003(bankNum);
		
	}else if (mapper == 30){
		Mapper030(bankNum);
	}
	
	std::cout << std::hex << addr << " map " << +bankNum << " " << +mapper001.sRegister << std::endl;
}

void Mapper::PrgLowBank2(int bank16k){
	*prgRom1 = prgRom + bank16k * sizeof(char);
	*prgRom2 = *prgRom1 + 0x2000 * sizeof(char);
}

void Mapper::PrgHighBank2(int bank16k){
	*prgRom3 = prgRom + bank16k * sizeof(char);
	*prgRom4 = *prgRom3 + 0x2000 * sizeof(char);
}

void Mapper::ChrLowBank2(int bank4k){
	ppuClass->rom1 = chrRom + bank4k * sizeof(char);
	ppuClass->rom2  = ppuClass->rom1 + 0x0800 * sizeof(char);	
}

void Mapper::ChrHighBank2(int bank4k){
	ppuClass->rom3 = chrRom + bank4k * sizeof(char);
	ppuClass->rom4  = ppuClass->rom3 + 0x0800 * sizeof(char);
}

void Mapper::ChrBank4(int bank8k){
	ChrLowBank2(bank8k);
	ppuClass->rom3  = ppuClass->rom2 + 0x0800 * sizeof(char);
	ppuClass->rom4  = ppuClass->rom3 + 0x0800 * sizeof(char);	
}

void Mapper::Mapper001(char bankNum, unsigned short addr){
	if ((bankNum & 0x80) == 0){
		mapper001.sRegister |= ((bankNum & 0x01) << mapper001.counter);

		if (mapper001.counter != 4){
			++mapper001.counter;
			return;
			
		}else{
			mapper001.counter = 0;
		}
	}else{
		mapper001.sRegister |= 0x0c;
		mapper001.counter = 0;
		addr = 0x8000;
	}
	std::cout << +addr << std::endl;
	if (addr < 0xa000){
		bool bit0, bit1;
		bit0 = CheckBit(mapper001.sRegister, 0);
		bit1 = CheckBit(mapper001.sRegister, 1);
		ppuClass->vMirror = ! (bit0 & bit1);
		ppuClass->hMirror = bit0 | (! bit1);
		
		if ((mapper001.sRegister & 0x03) == 0x01){
			ppuClass->name = 0b110000000000;
		}else{
			ppuClass->name = 0;
		}

		mapper001.pBankMode = mapper001.sRegister & 0b00001100;
		mapper001.cBankMode = mapper001.sRegister & 0b00010000;			std::cout << std::hex << +mapper001.pBankMode << std::endl;
		
		if (mapper001.pBankMode == 0b00001000){
			PrgLowBank2(0);
			PrgHighBank2(mapper001.pBankNum);
		}else if (mapper001.pBankMode == 0b00001100){
			PrgLowBank2(mapper001.pBankNum);
			PrgHighBank2((C2I(header[4]) - 1) << 14);
		}else {
			PrgLowBank2(mapper001.pBankNum & 0xffff8000);
			PrgHighBank2(mapper001.pBankNum | 0x00004000);
		}
		
		if (mapper001.cBankMode == 0b00010000){
			ChrLowBank2(mapper001.cBankLowNum);
			ChrHighBank2(mapper001.cBankHighNum);

		}else {
			ChrBank4(mapper001.cBankLowNum & 0xffffe000);
		}

	}else if (addr < 0xc000){
		mapper001.cBankLowNum = static_cast<int>(mapper001.sRegister) << 12;
		if (mapper001.cBankMode == 0){
			ChrBank4(mapper001.cBankLowNum & 0xffffe000);
		}else{
			ChrLowBank2(mapper001.cBankLowNum);
		}
		
	}else if (addr < 0xe000){
		mapper001.cBankHighNum = static_cast<int>(mapper001.sRegister) << 12;
		if (mapper001.cBankMode != 0){
			ChrHighBank2(mapper001.cBankHighNum);
		}
		
	}else{
		mapper001.pBankNum = static_cast<int>(mapper001.sRegister) << 14;
		if (mapper001.pBankMode == 0x08)
		{
			PrgHighBank2(mapper001.pBankNum);

		}else if (mapper001.pBankMode == 0x0c){
			PrgLowBank2(mapper001.pBankNum);
			
		}else{
			PrgLowBank2(mapper001.pBankNum & 0xffff8000);
			PrgHighBank2(mapper001.pBankNum | 0x00004000);
		}
	}
	mapper001.sRegister = 0;
}

void Mapper::Mapper002(char bankNum){
	PrgLowBank2(C2I(bankNum) << 14);
}

void Mapper::Mapper003(char bankNum){
	ChrBank4(C2I(bankNum) << 13);
}

void Mapper::Mapper030(char bankNum){
	int prgBank;
	int chrBank;
	prgBank = static_cast<int>(bankNum & 0x1f) << 14;
	chrBank = (static_cast<int>(bankNum) & 0x60) << 8;
	PrgLowBank2(prgBank);
	ChrBank4(chrBank); 
}

	
