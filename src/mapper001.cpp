/*mapperのソース*/

#include "mapper.hpp"
#include "cartridge.hpp"

#define P_ROM_BANK_SIZE 16 * 1024
#define C_ROM_BANK_SIZE 8 * 1024
#define RAM_WINDOW 8 * 1024
#define CHR_WINDOW 0x400


void Mapper::mapper001prg(uint8_t num){
	if (mapperStatus.pBankMode == 0x0c){
		PrgBank2(static_cast<int>(num) * P_ROM_BANK_SIZE, 0);
		PrgBank2(crg->romFeature->pRomSize - P_ROM_BANK_SIZE, 1);
		
	}else if(mapperStatus.pBankMode == 0x08){
		PrgBank2(0, 0);
		PrgBank2(static_cast<int>(num) * P_ROM_BANK_SIZE, 1);
	}else{
		int i;
		i = static_cast<int>(num >> 1);
		i *= P_ROM_BANK_SIZE;
		PrgBank2(i, 0);
		PrgBank2(i + (P_ROM_BANK_SIZE >> 1), 0);
	}
}

void Mapper::mapper001chr(uint8_t num1, uint8_t num2){
	if (CheckBit(mapperStatus.cBankMode, 4)){
		ChrBank4(static_cast<int>(num1) * (CHR_WINDOW << 2), 0);
		ChrBank4(static_cast<int>(num2) * (CHR_WINDOW << 2), 1);
	}else{
		ChrBank8(static_cast<int>(num1 >> 1 ) * C_ROM_BANK_SIZE);
	}
}

void Mapper::SxRom(){
	int cSize;
	int pSize;
	
	cSize = crg->romFeature->cRomSize;
	pSize = crg->romFeature->pRamSize;

	if (cSize == 16 * C_ROM_BANK_SIZE){
		SxRomE = &Mapper::ChrA16;
	}else{
		if (crg->romFeature->pRomSize == 32 * P_ROM_BANK_SIZE){
			SxRomE = &Mapper::PrgA18;
		}else{
			SxRomE = &Mapper::Dummy;
		}
	}
	
	if (cSize >= 8 * C_ROM_BANK_SIZE){
		SxRomD = &Mapper::ChrA15;
	}else{
		if (pSize == 4 * RAM_WINDOW){
			SxRomD = &Mapper::PrgA14;
		}else if (pSize == 2 * RAM_WINDOW){
			SxRomD = &Mapper::PrgA13;
		}else{
			SxRomD = &Mapper::Dummy;
		}
	}
	
	if (cSize >= 4 * C_ROM_BANK_SIZE){
		SxRomC = &Mapper::ChrA14;
	}else{
		SxRomC = &Mapper::Dummy;
	}
	
	if (cSize >= 2 * C_ROM_BANK_SIZE){
		SxRomB = &Mapper::ChrA13;
	}else{
		SxRomB = &Mapper::Dummy;
	}
	
}

uint8_t Mapper::ChrA13(uint8_t value){
	return value & 0x02;
}

uint8_t Mapper::ChrA14(uint8_t value){
	return value & 0x04;
}

uint8_t Mapper::ChrA15(uint8_t value){
	return value & 0x08;
}

uint8_t Mapper::ChrA16(uint8_t value){
	return value & 0x10;
}

uint8_t Mapper::PrgA18(uint8_t value){
	mapperStatus.bankNum[2] &= 0x0f;
	mapperStatus.bankNum[2] |= value & 0x10;
	mapper001prg(mapperStatus.bankNum[2]);
	return 0;
}

uint8_t Mapper::PrgA13(uint8_t value){
	mapperStatus.bankNum[3] = (value >> 2) & 0x01;
	prgRam = crg->prgRam + mapperStatus.bankNum[3] * RAM_WINDOW;
	return 0;
}

uint8_t Mapper::PrgA14(uint8_t value){
	mapperStatus.bankNum[3] = (value >> 2) & 0x03;
	prgRam = crg->prgRam + mapperStatus.bankNum[3] * RAM_WINDOW;
	return 0;
}

uint8_t Mapper::Dummy(uint8_t value){
	return 0;
}

void Mapper::Mapper001(uint8_t bankNum, uint16_t addr){
	if (CheckBit(bankNum, 7)){
		mapperStatus.counter = 0;
		mapperStatus.control |= 0x0c;
		mapperStatus.sRegister = mapperStatus.control;
		addr = 0x8000;
	}else{
		mapperStatus.sRegister |= ((bankNum & 0x01) << mapperStatus.counter);

		if (mapperStatus.counter != 4){
			++mapperStatus.counter;
			return;
			
		}else{
			mapperStatus.counter = 0;
		}
	}

	if (addr < 0xa000){
		/*コントロールレジスタの処理*/
		mapperStatus.control = mapperStatus.sRegister;
		
		/*下位2bit(ミラー)*/
		uint8_t mirror = mapperStatus.control & 0x03;
		if(mirror == 3){
			HMirror();
		}else if(mirror == 2){
			VMirror();
		}else{
			mirror &= 0x01;
			OneScreen(mirror);
		}
		
		/*PRGバンクの処理*/
		mapperStatus.pBankMode = mapperStatus.sRegister & 0x0c;
		mapper001prg(mapperStatus.bankNum[2]);
		
		/*CHRバンクの処理*/
		mapperStatus.cBankMode = mapperStatus.sRegister & 0x10;
		mapper001chr(mapperStatus.bankNum[0], mapperStatus.bankNum[1]);

	/*CHRバンク1,2の設定*/
	}else if (addr < 0xe000){
		uint8_t temp;
		addr >>= 14;
		addr &= 0x01;
		temp = mapperStatus.sRegister & 0x01;
		temp |= (this->*SxRomB)(mapperStatus.sRegister);
		temp |= (this->*SxRomC)(mapperStatus.sRegister);
		temp |= (this->*SxRomD)(mapperStatus.sRegister);
		temp |= (this->*SxRomE)(mapperStatus.sRegister);

		mapperStatus.bankNum[addr] = temp;
		mapper001chr(mapperStatus.bankNum[0], mapperStatus.bankNum[1]);

	/*PRGバンクの設定*/
	}else{
		mapperStatus.bankNum[2] &= 0xf0;
		mapperStatus.bankNum[2] |= mapperStatus.sRegister & 0x0f;
		mapper001prg(mapperStatus.bankNum[2]);
	}
	mapperStatus.sRegister = 0;
}

