/*IOポートのソース*/
#include "ioport.hpp"
#include "ppu.hpp"
#include <iostream>

IOPort::IOPort(Ppu *PPU, uint8_t *key){
	int i;
	ppu = PPU;
	controller = key;
	
	for (i = 0; i < 0x30; ++i){
		ioReg[i] = nullptr;
	}
			
	ioReg[0] = &ppu->ppuCtr;
	ioReg[1] = &ppu->ppuMask;
	ioReg[2] = &ppu->ppuStatus;
	ioReg[3] = &ppu->oamAddr;
	ioReg[4] = &ppu->oamData;
	ioReg[5] = &ppu->ppuScroll;
	ioReg[6] = &ppu->ppuAddr;
	ioReg[7] = &ppu->ppuData;
	
	ioReg[0x24] = &ppu->dmaData;
	
	ioReg[0x26] = &padStatus;
	padStatus = 0xff;
	
	//ioReg[0x27] = nullptr;
	
	A12 = &Ppu::Ppu::A12;
	
}

/*ioレジスタの読み取り*/
uint8_t IOPort::IORead(uint16_t addr){
	if (addr < 0x4000){
		addr &= 0x0007;
	}
	
	if (addr == 0x2){
		uint8_t value;
		ppu->addr.w = false;
		value = ppu->ppuStatus;
		ppu->ppuStatus &= 0x7f;
		return value;
	
	}else if (addr ==0x4){
		ppu->oamData = ppu->spriteTable [ppu->oamAddr];
		return ppu->oamData;
	
	}else if (addr == 0x7){
		if((ppu->addr.v & 0x3fff) >= 0x3f00){
			readBuffer = *ppu->MemoryMap(ppu->addr.v & 0x2fff);
			ppu->ppuData = *ppu->MemoryMap(ppu->addr.v);
		}else{
			ppu->ppuData = readBuffer;
			readBuffer = *ppu->MemoryMap(ppu->addr.v);
		}
		if(CheckBit(ppu->ppuCtr, 2)){
			ppu->addr.v = (ppu->*(ppu->*A12))(ppu->addr.v + 32);
		}else{
			ppu->addr.v = (ppu->*(ppu->*A12))(ppu->addr.v + 1);
		}
		ppu->addr.v &= 0x7fff;
		return ppu->ppuData;

	}else if (addr == 0x4016){
		padStatus &= 0xfe;
		padStatus |= (padTemp & 0x01);
		padTemp >>= 1;
		padTemp |= 0b10000000;

		return padStatus;

	}
	return 0;
}

/*ioレジスタに書き込み*/
void IOPort::IOWrite(uint8_t value, uint16_t addr){
	if (addr < 0x4000){
		addr &= 0x0007;
	}

	if (addr == 0x0){
		if (CheckBit(ppu->ppuStatus, 7)){
			if (CheckBit(value & (0xff ^ ppu->ppuCtr), 7)){
				cpuIRQ->irqStr.nmi = true;
			}
		}
		ppu->ppuCtr = value;
		ppu->addr.t &= 0x73ff;
		ppu->addr.t |= ((static_cast<uint16_t>(value) << 10) & 0x0c00);
		
	}else if (addr == 0x1){
		ppu->ppuMask = value;
		
	}else if (addr == 0x3){
		ppu->oamAddr = value;

	}else if (addr == 0x4){
		ppu->spriteTable[ppu->oamAddr] = value;
		++ppu->oamAddr;
		
	}else if (addr == 0x5){
		uint16_t temp;
		temp = static_cast<uint16_t>(value);
		if(ppu->addr.w){
			ppu->addr.t &= 0x0c1f;
			ppu->addr.t |= ((temp << 12) & 0x7000);
			ppu->addr.t |= ((temp << 2) & 0x03e0);
			ppu->addr.w = false;
		}else{
			ppu->addr.t &= 0x7fe0;
			ppu->addr.t |= (temp >> 3) & 0x001f;
			ppu->addr.x = temp & 0x0007;
			ppu->addr.w = true;
		}
		ppu->ppuScroll = value;
		
	}else if (addr == 0x6){
		if(ppu->addr.w){
			/*ppuメモリの下位2ビットアドレスに上位2ビットをマージ*/
			ppu->addr.t &= 0x7f00;
			ppu->addr.t |=  static_cast<uint16_t>(value);
			ppu->addr.v = (ppu->*(ppu->*A12))(ppu->addr.t);
			ppu->addr.w = false;
		}else{
			/*ppuメモリの上位2ビットアドレス*/
			ppu->addr.t &= 0x00ff;
			ppu->addr.t |= static_cast<uint16_t>(value & 0x3f) << 8;
			ppu->addr.w = true;
		}

	}else if (addr == 0x7){
		*ppu->MemoryMap(ppu->addr.v) = value;
		if(CheckBit(ppu->ppuCtr, 2)){
			ppu->addr.v = (ppu->*(ppu->*A12))(ppu->addr.v + 32);
		}else{
			ppu->addr.v = (ppu->*(ppu->*A12))(ppu->addr.v + 1);
		}
		ppu->addr.v &= 0x7fff;
		
	}else if (addr == 0x4014){
		ppu->dmaData = value;
		dma = true;
		
	}else if (addr == 0x4016){
		/*アクセス前後で値が1から0に変わったことを判定*/
		if (CheckBit(padStatus, 0) && (! CheckBit(value, 0))){
			/*ボタンの状態をラッチする*/
			padTemp = *controller;

		}
		padStatus = value;
		
	}
}

void IOPort::DMAFunc(uint8_t *addr){
	for (int i = 0; i < 256; i++){
	ppu->spriteTable[i] = addr[i];
	
	}
	dma = false;

}

