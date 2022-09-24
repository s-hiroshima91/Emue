/*IOポートのソース*/
#include "ioport.h"
#include "ppu.h"
#include "common.h"
#include <iostream>

IOPort::IOPort(){
	ioFlg = 0xff;
}

/*ppuレジスタにアクセスしたときの関数*/


void IOPort::IOFunc(int wFlg){
	unsigned short value;
	if (ioFlg > 0x0007){
		return;
	}
	
	if (ioFlg == 0x0007){
		if (wFlg){
			*ppuClass->MemoryMap(ppuClass->addr.v) = ppuIO[0x0007];
		}else{
			readBuffer = *ppuClass->MemoryMap(ppuClass->addr.v);
		}
		ppuIO[0x0007] = readBuffer;
		if((ppuIO[0x0000] & 0x04) == 0x04){
			ppuClass->addr.v += 32;
		}else{
			ppuClass->addr.v += 1;
		}
		ppuClass->addr.v &= 0x7fff;
		
	}else if (ioFlg == 0x0006){
		if(ppuClass->addr.w){
			/*ppuメモリの下位2ビットアドレスに上位2ビットをマージ*/
			ppuClass->addr.t &= 0x7f00;
			ppuClass->addr.t |=  C2US(ppuIO[0x0006]);
			ppuClass->addr.v = ppuClass->addr.t;
			ppuClass->addr.w = false;
		}else{
			/*ppuメモリの上位2ビットアドレス*/
			ppuClass->addr.t &= 0x00ff;
			ppuClass->addr.t |= ((static_cast<unsigned short>(ppuIO[0x0006]) & 0x3f) << 8);
			ppuClass->addr.w = true;
		}
//		std::cout << std::hex << ppuClass->addr.w <<" 06 " << +ppuIO[0x0006] << std::endl;
		
	}else if (ioFlg == 0x0005){
		if(ppuClass->addr.w){
			value = static_cast<unsigned short>(ppuIO[0x0005]);
			ppuClass->addr.t &= 0x0c1f;
			ppuClass->addr.t |= ((value << 12) & 0x7000);
			ppuClass->addr.t |= ((value << 2) & 0x03e0);
			ppuClass->addr.w = false;
		}else{
			ppuClass->addr.t &= 0x7fe0;
			ppuClass->addr.t |= (static_cast<unsigned short>(ppuIO[0x0005] >> 3) & 0x001f);
			ppuClass->addr.x = static_cast<unsigned short>(ppuIO[0x0005]) & 0x0007;
			ppuClass->addr.w = true;
		}
		
//		std::cout << std::hex << ppuClass->addr.w <<" 05 " << +ppuIO[0x0005] << std::endl;
		
	}else if (ioFlg == 0x0004){
		ppuClass->spriteTable[ppuIO[0x0003]] = ppuIO[0x0004];
		ppuIO[0x0003] += wFlg;
		ppuIO[0x0004] = ppuClass->spriteTable[ppuIO[0x0003]];
		
/*	}else if (ioFlg == 0x0003){*/

	}else if (ioFlg == 0x0002){
		ppuIO[0x0002] &= 0x7f;
		ppuClass->addr.w = false;
//		std::cout << std::hex << ppuClass->addr.w << " 02 " << +ppuIO[0x0002] << std::endl;
		
	}else if (ioFlg == 0x0001){
		ppuClass->ctrRegister2 = ppuIO[0x0001];
		
	}else if (ioFlg == 0x0000){
		ppuClass->ctrRegister1 = ppuIO[0x0000];
		ppuClass->addr.t &= 0x73ff;
		ppuClass->addr.t |= ((static_cast<unsigned short>(ppuIO[0x0000]) << 10) & 0x0c00);
//		std::cout << std::hex << ppuClass->addr.w << " 00 " << +ppuIO[0x0000] << std::endl;
	}
	ioFlg = 0xff;
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

