/*mapperのソース*/

#include "mapper.hpp"
#include "cartridge.hpp"

#define P_ROM_BANK_SIZE 16 * 1024
#define C_ROM_BANK_SIZE 8 * 1024

void Mapper::Mapper000(uint8_t bankNum, uint16_t addr){
	/*なにもしない*/
}

void Mapper::Mapper002(uint8_t bankNum, uint16_t addr){
	PrgBank2(static_cast<int>(bankNum) * P_ROM_BANK_SIZE, 0);
}

void Mapper::Mapper003(uint8_t bankNum, uint16_t addr){
	ChrBank8(static_cast<int>(bankNum) * C_ROM_BANK_SIZE);
}

void Mapper::Mapper030Bat(uint8_t bankNum, uint16_t addr){
	int prgBank;
	int chrBank;
	
	if (addr >= 0xc000){
		prgBank = static_cast<int>(bankNum & 0x1f);
		chrBank = static_cast<int>(bankNum & 0x60) >> 5;
		PrgBank2(prgBank * P_ROM_BANK_SIZE, 0);
		ChrBank8(chrBank * C_ROM_BANK_SIZE << 1);
		return;
	}

	if (addr == 0x9555){
		if ((flash == 0) || (flash == 3)){
			if (bankNum == 0xaa){
				++flash;
				return;
			}
		}else if (flash == 2){
			if (bankNum == 0x80){
				++flash;
				return;
			}else if (bankNum == 0xa0){
				flash = 10;
				return;
			}
		}
	}
	
	if(addr == 0xaaaa){			
		if((flash == 1) || (flash == 4)){
			if (bankNum == 0x55){
				++flash;
				return;
			}
		}
	}
	
	if ((flash == 5) && (bankNum == 0x30)){
		if ((addr & 0x0fff) == 0){
			uint8_t *ptr;
			ptr = cpuMap[0] + (addr & 0x3000);
			for (int i = 0; i < 0x1000; ++i){
				*ptr = 0;
				++ptr;
			}
		}
		
	}else if (flash == 10){
		*(cpuMap[0] + (addr & 0x3fff)) = bankNum;
	}
	
	flash = 0;		
}

void Mapper::Mapper030NoBat(uint8_t bankNum, uint16_t addr){
	int prgBank;
	int chrBank;
	prgBank = static_cast<int>(bankNum & 0x1f);
	chrBank = static_cast<int>(bankNum & 0x60) >> 5;
	PrgBank2(prgBank * P_ROM_BANK_SIZE, 0);
	ChrBank8(chrBank * C_ROM_BANK_SIZE << 1);
}
