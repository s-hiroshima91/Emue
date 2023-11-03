/*mapperのソース*/

#include "mapper.hpp"
#include "cartridge.hpp"
#include <iostream>

#define P_ROM_BANK_SIZE 16 * 1024
#define C_ROM_BANK_SIZE 8 * 1024
#define RAM_WINDOW 8 * 1024

#define CHR_WINDOW 0x400
#define CHR_SCREEN 0x400

Mapper::Mapper(Cartridge *cartridge){
	crg = cartridge;
	mapperNum = crg->romFeature->mapper;
	
	bool saveDate = false;	
	
	if (mapperNum == 0){
		MapFunc = &Mapper::Mapper000;

	}else if(mapperNum == 1){
		MapFunc = &Mapper::Mapper001;
		
		SxRom();

		if (crg->romFeature->flg.battery){
			if (crg->romFeature->pRamSize == 0){
				crg->romFeature->pRamSize = 4 * RAM_WINDOW;
			}
		}

	}else if(mapperNum == 2){
		MapFunc = &Mapper::Mapper002;

	}else if(mapperNum == 3){
		MapFunc = &Mapper::Mapper003;
		
	}else if(mapperNum == 4){
		MapFunc = &Mapper::Mapper004;
		
		mapperStatus.bankNum[8] = 
		static_cast<uint8_t>(crg->romFeature->pRomSize / (P_ROM_BANK_SIZE >> 1)) - 2;
		if (crg->romFeature->flg.battery){
			if (crg->romFeature->pRamSize == 0){
				crg->romFeature->pRamSize = RAM_WINDOW;
			}
		}
		
	}else if (mapperNum == 30){
		if (crg->romFeature->flg.battery){
			MapFunc = &Mapper::Mapper030Bat;
			crg->romFeature->pRamSize = crg->romFeature->pRomSize;
			saveDate = true;
		}else{
			MapFunc = &Mapper::Mapper030NoBat;
		}
		crg->romFeature->cRamSize = C_ROM_BANK_SIZE * 4;
	}
	
	crg->ReadRom();
	saveDate = crg->ReadRam();
	
	if (mapperNum == 30){
		if(saveDate){
			delete crg->prgRom;
			crg->romFeature->flg.pRom = false;
			crg->prgRom = crg->prgRam;
		}else{
			delete crg->prgRam;
			crg->romFeature->flg.pRam = false;
			crg->prgRam = crg->prgRom;
		}
	}
	
	cpuMap[0] = crg->prgRom;
	cpuMap[1] = cpuMap[0] + 0x2000;
	cpuMap[2] = crg->prgRom + crg->romFeature->pRomSize - P_ROM_BANK_SIZE;
	cpuMap[3] = cpuMap[2] + 0x2000;
	
	prgRam = crg->prgRam;
	
	int i;
	for (i = 0; i < 8; ++i){
		ppuMap[i] = crg->chrRom + i * CHR_WINDOW;
	}
	
	if (crg->romFeature->flg.fourScreen){
		FourScreen();
	}else{
		if (crg->romFeature->flg.mirror){
			VMirror();
		}else{
			HMirror();
		}
	}
	std::cout<<crg->romFeature ->mapper;
		
	
	//extRam = eRamDate;
	
/*	if (mapper == 30){
		chrRom =  static_cast<char*>(calloc(32 * 1024, sizeof(char)));
		ChrBank8(0);
	}*/


}

void Mapper::MapperCall(uint8_t bankNum, uint16_t addr){
	(this->*MapFunc)(bankNum, addr);
}

void Mapper::PrgBank2(int bank16k, int i){
	i <<= 1;
	cpuMap[i] = crg->prgRom + bank16k;
	cpuMap[i + 1] = cpuMap[i] + (P_ROM_BANK_SIZE >> 1);

}

void Mapper::ChrBank2(int bank2k, int i){
	i <<= 1;
	ppuMap[i] = crg->chrRom + bank2k;
	ppuMap[i + 1] = ppuMap[i] + CHR_WINDOW;
}

void Mapper::ChrBank4(int bank4k, int i){
	i <<= 1;
	ChrBank2(bank4k, i);
	ChrBank2(bank4k + (CHR_WINDOW <<1), i + 1);	
}

void Mapper::ChrBank8(int bank8k){
	ChrBank4(bank8k, 0);
	ChrBank4(bank8k + (CHR_WINDOW << 2), 1);
}



void Mapper::FourScreen(){
	for (int i = 0; i < 4; ++i){
		screen[i] = ppuTable + i *CHR_SCREEN;
	}
}

void Mapper::VMirror(){
	screen[0] = ppuTable;
	screen[1] = ppuTable + CHR_SCREEN;
	screen[2] = screen[0];
	screen[3] = screen[1];
}

void Mapper::HMirror(){
	screen[0] = ppuTable;
	screen[2] = ppuTable + CHR_SCREEN;
	screen[1] = screen[0];
	screen[3] = screen[2];
}

void Mapper::OneScreen(int i){
	screen[0] =  ppuTable + CHR_SCREEN * i;
	screen[1] = screen[0];
	screen[2] = screen[0];
	screen[3] = screen[0];
}

