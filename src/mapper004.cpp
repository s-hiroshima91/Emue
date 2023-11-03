/*mapperのソース*/

#include "mapper.hpp"
#include "cartridge.hpp"

#define P_ROM_BANK_SIZE 16 * 1024
#define CHR_WINDOW 0x400

void Mapper::MapperIRQ(){
	if (clearFlg){
		clearFlg = false;
		irqCounter = 0;
	}
	
	if(irqCounter == 0){
		irqCounter = irq;

	}else{
		--irqCounter;
	}
	
	if (irqCounter == 0){
		cpuIrq->irqStr.irbr |= irqEnable;
	}
	/*else if (irqCounter == 0xff){
		
	}*/
}


void Mapper::Mapper004(uint8_t bankNum, uint16_t addr){

	if (addr < 0xa000){
		if((addr & 0x0001) == 0){
			mapperStatus.pBankMode = bankNum & 0x40;
			mapperStatus.cBankMode = bankNum & 0x80;
			mapperStatus.sRegister = bankNum & 0x07;
			
		}else{
			mapperStatus.bankNum[mapperStatus.sRegister] = bankNum;
		}
		
		int init, delta;
		if(mapperStatus.pBankMode == 0){
			init = 6;
			delta = 1;
		}else{
			init = 8;
			delta = -1;
		}
		for(int i = 0; i < 3; ++i){
			cpuMap[i] = crg->prgRom + static_cast<int>(mapperStatus.bankNum[init] & 0x3f) * (P_ROM_BANK_SIZE >> 1);
			init += delta;
		}
		
		if(mapperStatus.cBankMode == 0){
			init = 0;
			delta = 2;
		}else{
			init = 2;
			delta = -2;
		}
		
		for (int i = 0; i < 2; ++i){
				ChrBank2(static_cast<int>(mapperStatus.bankNum[i] >> 1) * (CHR_WINDOW << 1), init + i);
		}
		for (int i = 2; i < 6; ++i){
			ppuMap[i + delta] = crg->chrRom + static_cast<int>(mapperStatus.bankNum[i]) * CHR_WINDOW;	
		}
		
	}else if(addr < 0xc000){
		if((addr & 0x0001) == 0){
			if (crg->romFeature->flg.fourScreen){
				return;
			}
			if(CheckBit(bankNum, 0)){
				HMirror();
			}else{
				VMirror();
			}
		}else{
			/*Ram書き込み無効*/
		}
			
	}else if(addr < 0xe000){ 
		if((addr & 0x0001) == 0){
			irq = static_cast<int>(bankNum);
			
		}else{
			clearFlg = true;
		}

	}else{
		if((addr & 0x0001) == 0){
			irqEnable = false;
			cpuIrq->irqStr.irbr = false;
		}else{
			irqEnable = true;
			//cpuIrq->irqStr.irbr = false;
		}
	}
}
