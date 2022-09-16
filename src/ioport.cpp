/*IOポートのソース*/
#include "ioport.h"
#include "ppu.h"
#include "common.h"
#include <iostream>

IOPort::IOPort(){
	ioFlg = 0;
	writeAddr = 0;
	temp = 0;
}

/*ioポートにアクセスがあったことを管理する関数*/
void IOPort::IOFlg(unsigned short addr){
	if (addr == 0x0000){
		ioFlg = 0x10;
		
	}else if (addr == 0x0001){
		ioFlg = 0x20;
		
	}else if (addr == 0x0002){
		ioFlg = 0x60;
		
	}else if (addr == 0x0003){
		ioFlg = 0x30;
		
	}else if (addr == 0x0004){
		ioFlg = 0x40;
		
	}else if (addr == 0x0005){
		ioFlg &= 0b0001;
		ioFlg += 0b0001;
		
	}else if(addr == 0x0006){
		ioFlg &= 0b0100;
		ioFlg += 0b0100;
		
	}else if (addr == 0x0007){
		ioFlg = 0x50;
		ppuIO[0x0007] = *ppuClass->MemoryMap(writeAddr);
		
	}else{
		ioFlg = 0;
	}
}

/*ppuレジスタにアクセスしたときの関数*/
void IOPort::IOFunc(){
	bool sFlg;
	char value;
	if ((ioFlg & 0x80) != 0){
		return;
	}
	
	if (ioFlg == 0x10){
		ppuClass->ctrRegister1 = ppuIO[0x0000];
		ioFlg = 0;
		
	}else if (ioFlg == 0x20){
		ppuClass->ctrRegister2 = ppuIO[0x0001];
		ioFlg = 0;
		
	}else if (ioFlg == 0x30){
		ioFlg = 0;
		
	}else if (ioFlg == 0x40){
		ppuClass->spriteTable[ppuIO[0x0003]] = ppuIO[0x0004];
		ppuIO[0x0003] += 1;
		ioFlg = 0;	
		
	}else if ((ioFlg & 0b00000011) != 0){
		/*2アクセスするので2ビットで管理*/
		ppuClass->scroll[ioFlg - 1] = ppuIO[0x0005];
		ioFlg |= 0x80;
		
	}else if (ioFlg == 0b00000100){
		/*2回アクセスがあるので下と合わせて2ビットで管理*/
		/*ppuメモリの上位2ビットアドレス*/
		writeAddr = ppuIO[0x0006];
		writeAddr <<= 8;
		ioFlg |= 0x80;
		
	}else if (ioFlg == 0b00001000){
		/*2回アクセスがあるので上と合わせて2ビットで管理*/
		/*ppuメモリの下位2ビットアドレスに上位2ビットをマージ*/
		writeAddr |= (ppuIO[0x0006] & 0x00ff);
		ioFlg |= 0x80;
		
	}else if (ioFlg == 0x50){
		*ppuClass->MemoryMap(writeAddr) = ppuIO[0x0007];
		sFlg = CheckBit(ppuIO[0x0000], 2);
		writeAddr += (1 << (sFlg * 5));
		writeAddr &= 0x3fff;
		ioFlg = 0;
		
	}else if (ioFlg == 0x60){
		ppuIO[0x0002] &= 0x7f;
		ioFlg = 0;
	}
}		

/*パッドioにアクセスしたことを判定*/
void IOPort::PadFlg(unsigned short addr){
	if (addr == 0x0016){
		temp = CheckBit(padIO[addr], 0);
		/*アクセスしたときのビット0の値を保持*/
		padFlg = true;
	}else if (addr == 0x0014){
		dma = true;

	}
}
/*パッドioにアクセスしたときの処理*/
void IOPort::PadFunc(){
	bool flg;
	if (padFlg == true){
		flg = CheckBit(padIO[0x0016], 0);
		/*アクセス前後で値が1から0に変わったことを判定*/
		if ((temp == true) && (flg == false)){
			/*ボタンの状態を書き込む*/
			padStatus = padTemp;
		}
		/*次回読み込みの準備*/
		padFlg = false;
		padIO[0x0016] &= 0xfe;
		padIO[0x0016] |= (padStatus & 0x01);
		padStatus >>= 1;
		padStatus |= 0b10000000;
	}
}

void IOPort::DMAFunc(char *addr){
	for (int i = 0; i < 256; i++){
	ppuClass->spriteTable[i] = addr[i];
	
	}
	dma = false;
}

