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
		ChrBank8(0);
	}
	
}

void Mapper::BankSelect(char bankNum, unsigned short addr){

	if(mapper == 1){
		Mapper001(bankNum, addr);
		
	}else if(mapper == 2){
		Mapper002(bankNum);

	}else if(mapper == 3){
		Mapper003(bankNum);
		
	}else if(mapper == 4){
		Mapper004(bankNum, addr);
		
	}else if (mapper == 30){
		Mapper030(bankNum);
	}
	
//	std::cout << std::hex << addr << " map " << +bankNum << " " << +status.sRegister << std::endl;
}

void Mapper::PrgLowBank2(int bank16k){
	*prgRom1 = prgRom + bank16k * sizeof(char);
	*prgRom2 = *prgRom1 + 0x2000 * sizeof(char);
}

void Mapper::PrgHighBank2(int bank16k){
	*prgRom3 = prgRom + bank16k * sizeof(char);
	*prgRom4 = *prgRom3 + 0x2000 * sizeof(char);
}

void Mapper::Chr1stBank2(int bank2k){
	ppuClass->rom1 = chrRom + bank2k * sizeof(char);
	ppuClass->rom2  = ppuClass->rom1 + 0x0400 * sizeof(char);	
}

void Mapper::Chr2ndBank2(int bank2k){
	ppuClass->rom3 = chrRom + bank2k * sizeof(char);
	ppuClass->rom4  = ppuClass->rom3 + 0x0400 * sizeof(char);
}

void Mapper::Chr3rdBank2(int bank2k){
	ppuClass->rom5 = chrRom + bank2k * sizeof(char);
	ppuClass->rom6  = ppuClass->rom5 + 0x0400 * sizeof(char);	
}

void Mapper::Chr4thBank2(int bank2k){
	ppuClass->rom7 = chrRom + bank2k * sizeof(char);
	ppuClass->rom8  = ppuClass->rom7 + 0x0400 * sizeof(char);
}

void Mapper::ChrLowBank4(int bank4k){
	Chr1stBank2(bank4k);
	Chr2ndBank2(bank4k + 0x0800);	
}

void Mapper::ChrHighBank4(int bank4k){
	Chr3rdBank2(bank4k);
	Chr4thBank2(bank4k + 0x0800);	
}

void Mapper::ChrBank8(int bank8k){
	ChrLowBank4(bank8k);
	ChrHighBank4(bank8k + 0x1000);	
}

bool Mapper::MapperIRQ(bool irqFlg){
	if(mapper != 4){
		return irqFlg;
	}
	
	if(irqCounter == 0){
		irqCounter = irq;
		return irqFlg | irqEnable;
	}else{
		--irqCounter;
		return irqFlg;
	}
}

void Mapper::Mapper001(char bankNum, unsigned short addr){
	if ((bankNum & 0x80) == 0){
		status.sRegister |= ((bankNum & 0x01) << status.counter);

		if (status.counter != 4){
			++status.counter;
			return;
			
		}else{
			status.counter = 0;
		}
	}else{
		status.sRegister |= 0x0c;
		status.counter = 0;
		addr = 0x8000;
	}
//	std::cout << +addr << std::endl;
	if (addr < 0xa000){
		char mFlg = status.sRegister & 0x03;
		if(mFlg == 0){
			ppuClass->screen.a = &(ppuClass->ppuTable[0x0000]);
			ppuClass->screen.b = &(ppuClass->ppuTable[0x0000]);
			ppuClass->screen.c = &(ppuClass->ppuTable[0x0000]);
			ppuClass->screen.d = &(ppuClass->ppuTable[0x0000]);
		}else if(mFlg == 0x01){
			ppuClass->screen.a = &(ppuClass->ppuTable[0x0400]);
			ppuClass->screen.b = &(ppuClass->ppuTable[0x0400]);
			ppuClass->screen.c = &(ppuClass->ppuTable[0x0400]);
			ppuClass->screen.d = &(ppuClass->ppuTable[0x0400]);
		}else if(mFlg == 0x02){
			ppuClass->screen.a = &(ppuClass->ppuTable[0x0000]);
			ppuClass->screen.b = &(ppuClass->ppuTable[0x0400]);
			ppuClass->screen.c = &(ppuClass->ppuTable[0x0000]);
			ppuClass->screen.d = &(ppuClass->ppuTable[0x0400]);
		}else{
			ppuClass->screen.a = &(ppuClass->ppuTable[0x0000]);
			ppuClass->screen.b = &(ppuClass->ppuTable[0x0000]);
			ppuClass->screen.c = &(ppuClass->ppuTable[0x0400]);
			ppuClass->screen.d = &(ppuClass->ppuTable[0x0400]);
		}

		status.pBankMode = status.sRegister & 0b00001100;
		status.cBankMode = status.sRegister & 0b00010000;		
//		std::cout << std::hex << +status.pBankMode << std::endl;
		
		if (status.pBankMode == 0b00001000){
			PrgLowBank2(0);
			PrgHighBank2(status.bankNum[6]);
		}else if (status.pBankMode == 0b00001100){
			PrgLowBank2(status.bankNum[6]);
			PrgHighBank2((C2I(header[4]) - 1) << 14);
		}else {
			PrgLowBank2(status.bankNum[6] & 0xffff8000);
			PrgHighBank2(status.bankNum[6] | 0x00004000);
		}
		
		if (status.cBankMode == 0b00010000){
			ChrLowBank4(status.bankNum[0]);
			ChrHighBank4(status.bankNum[1]);

		}else {
			ChrBank8(status.bankNum[0] & 0xffffe000);
		}

	}else if (addr < 0xc000){
		status.bankNum[0] = static_cast<int>(status.sRegister) << 12;
		if (status.cBankMode == 0){
			ChrBank8(status.bankNum[0] & 0xffffe000);
		}else{
			ChrLowBank4(status.bankNum[0]);
		}
		
	}else if (addr < 0xe000){
		status.bankNum[1] = static_cast<int>(status.sRegister) << 12;
		if (status.cBankMode != 0){
			ChrHighBank4(status.bankNum[1]);
		}
		
	}else{
		status.bankNum[6] = static_cast<int>(status.sRegister) << 14;
		if (status.pBankMode == 0x08)
		{
			PrgHighBank2(status.bankNum[6]);

		}else if (status.pBankMode == 0x0c){
			PrgLowBank2(status.bankNum[6]);
			
		}else{
			PrgLowBank2(status.bankNum[6] & 0xffff8000);
			PrgHighBank2(status.bankNum[6] | 0x00004000);
		}
	}
	status.sRegister = 0;
}

void Mapper::Mapper002(char bankNum){
	PrgLowBank2(C2I(bankNum) << 14);
}

void Mapper::Mapper003(char bankNum){
	ChrBank8(C2I(bankNum) << 13);
}

void Mapper::Mapper004(char bankNum, unsigned short addr){

	if (addr < 0xa000){
		if((addr & 0x0001) == 0){
			status.pBankMode = static_cast<char>(CheckBit(bankNum, 6));
			status.cBankMode = static_cast<char>(CheckBit(bankNum, 7));
			status.sRegister = bankNum & 0x07;
			
		}else{
			int temp;
			temp = C2I(status.sRegister);
			if(temp <= 1){
				temp = (static_cast<int>(bankNum) & 0x000000fe) << 10;
				
			}else if(temp >= 6){
				temp = (static_cast<int>(bankNum) & 0x0000003f) << 13;
				
			}else{
				temp = C2I(bankNum) << 10;

			}
			status.bankNum[status.sRegister] = temp;
		}
		
		if(status.pBankMode == 0){
			*prgRom1 = prgRom + status.bankNum[6] * sizeof(char);
			*prgRom2 = prgRom + status.bankNum[7] * sizeof(char);
			PrgHighBank2((C2I(header[4]) - 1) <<14);
			
		}else{
			*prgRom1 = prgRom + ((C2I(header[4]) - 1) <<14) * sizeof(char);
			*prgRom2 = prgRom + status.bankNum[6] * sizeof(char);
			*prgRom3 = prgRom + status.bankNum[7] * sizeof(char);
			*prgRom4 = *prgRom1 + 0x2000 * sizeof(char);
		}
		
		if(status.cBankMode == 0){
			Chr1stBank2(status.bankNum[0]);
			Chr2ndBank2(status.bankNum[1]);
			ppuClass->rom5 = chrRom + status.bankNum[2] * sizeof(char);
			ppuClass->rom6 = chrRom + status.bankNum[3] * sizeof(char);
			ppuClass->rom7 = chrRom + status.bankNum[4] * sizeof(char);
			ppuClass->rom8 = chrRom + status.bankNum[5] * sizeof(char);
			
		}else{
			ppuClass->rom1 = chrRom + status.bankNum[2] * sizeof(char);
			ppuClass->rom2 = chrRom + status.bankNum[3] * sizeof(char);
			ppuClass->rom3 = chrRom + status.bankNum[4] * sizeof(char);
			ppuClass->rom4 = chrRom + status.bankNum[5] * sizeof(char);
			Chr3rdBank2(status.bankNum[0]);
			Chr4thBank2(status.bankNum[1]);
		}
		
	}else if(addr < 0xc000){
		if((addr & 0x0001) == 0){
			if(! CheckBit(bankNum, 0)){
				ppuClass->screen.b = ppuClass->screen.d;
				ppuClass->screen.c = ppuClass->screen.a;
			}else{
				ppuClass->screen.b = ppuClass->screen.a;
				ppuClass->screen.c = ppuClass->screen.d;
			}
		}
			
	}else if(addr < 0xe000){ 
		if((addr & 0x0001) == 0){
			irq = C2I(bankNum);
			
		}else{
			irqCounter = irq + 1;
		}

	}else{
		if((addr & 0x0001) == 0){
			irqEnable = false;
		}else{
			irqEnable = true;
		}
	}
}

void Mapper::Mapper030(char bankNum){
	int prgBank;
	int chrBank;
	prgBank = static_cast<int>(bankNum & 0x1f) << 14;
	chrBank = (static_cast<int>(bankNum) & 0x60) << 8;
	PrgLowBank2(prgBank);
	ChrBank8(chrBank); 
}

	
