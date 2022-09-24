/*ppuクラスのソース*/

#include "ppu.h"
#include "ioport.h"
#include "common.h"
#include <iostream>



/*場所からパレットを紐付ける関数*/
inline char PaletteNum(char value, unsigned short addr){
	unsigned short num;
	/*縦32lineで1単位なので
	下の二行で32で割って8かける操作*/
	num = addr >> 4;
	num &= 0x0004;
	
	/*横4ブロックの前半か後半を判定*/
	num += (addr & 0x0002);
	/*塊4ブロックのどこかによって8ビットの
何番目を取り出すか判定する*/
	value >>= num;
	value <<= 2;
	/*パレットの上位2ビットを返す*/
	return value & 0b00001100;
}

/*場所からパターンテーブルを決定するクラス*/
unsigned short Ppu::PrePattern(unsigned short vAddr, bool flg){
	unsigned short value;
	value = 0x2000 | (vAddr & 0x0fff);
	value = C2US(*MemoryMap(value));
	value <<= 4;
	value |= ((static_cast<unsigned short>(flg)) << 12);
	value += ((vAddr >> 12) & 0x0007);
	return value;
}

/*パレットの値に変換するクラス*/
char Ppu::PreConvColor(unsigned short vAddr){
	unsigned short addr;
	char value;
	
	addr = 0x23c0 | (vAddr & 0x0c00);
	addr |= ((vAddr >> 4) & 0x0038);
	addr |= ((vAddr >> 2) & 0x0007);
	
	value = *MemoryMap(addr);
	value = PaletteNum(value, vAddr);
	return value;
}

/*インストラクタ*/
Ppu::Ppu(char *romDate, char header6, IOPort *ioP){
	
	rom1 = romDate;
	rom2 = rom1 + 0x0400 * sizeof(char);
	rom3 = rom2 + 0x0400 * sizeof(char);
	rom4 = rom3 + 0x0400 * sizeof(char);
	rom5 = rom4 + 0x0400 * sizeof(char);
	rom6 = rom5 + 0x0400 * sizeof(char);
	rom7 = rom6 + 0x0400 * sizeof(char);
	rom8 = rom7 + 0x0400 * sizeof(char);
	
	vMirror = CheckBit(header6, 0);
	hMirror = !vMirror;
	ioPort = ioP;
	ioPort->ppuClass = this;
	lineCounter = 0x0000;
	ioPort->ppuIO[0x0002] = 0b10000000;
	
	screen.a = &ppuTable[0x0000];
	if(vMirror){
		screen.b = &ppuTable[0x0400];
		screen.c = screen.a;
	}else{
		screen.b = screen.a;
		screen.c = &ppuTable[0x0400];
	}
	screen.d = &ppuTable[0x0400];
	
}

/*メモリマップ*/
char *Ppu::MemoryMap(unsigned short addr){
	char *pointer;
	addr &= 0x3fff;
	if (addr < 0x0400){
		pointer = &rom1[addr];
		
	}else if (addr < 0x0800){
		addr -= 0x0400;
		pointer = &rom2[addr];
		
	}else if (addr < 0x0c00){
		addr -= 0x0800;
		pointer = &rom3[addr];
		
	}else if (addr < 0x1000){
		addr -= 0x0c00;
		pointer = &rom4[addr];
		
	}else if (addr < 0x1400){
		addr -= 0x1000;
		pointer = &rom5[addr];
		
	}else if (addr < 0x1800){
		addr -= 0x1400;
		pointer = &rom6[addr];
	
	}else if (addr < 0x1c00){
		addr -= 0x1800;
		pointer = &rom7[addr];
		
	}else if (addr < 0x2000){
		addr -= 0x1c00;
		pointer = &rom8[addr];
		
/*	}else if (addr < 0x3f00){
		addr -= 0x2000;
		addr %= 0x1000;
		pointer = &ppuTable[addr];*/
		
	}else if (addr < 0x3f00){
		addr -= 0x2000;
		addr %= 0x1000;
		if (addr < 0x0400){
			pointer = &screen.a[addr];
		}else if (addr < 0x0800){
			addr -= 0x0400;
			pointer = &screen.b[addr];
		}else if(addr < 0x0c00){
			addr -= 0x0800;
			pointer = &screen.c[addr];
		}else{
			addr -= 0x0c00;
			pointer = &screen.d[addr];
		}
		
	}else if (addr < 0x4000){
		addr -= 0x3f00;
		addr %= 0x0020;
		pointer = &ppuPalette[addr];
		
	}else{
		std::cout << "ppuerror 0x" << std::hex << addr << std::endl;
	}
	return pointer;
}

/*1line分のデータを配列に格納するクラス*/
void Ppu::CreateImg(char *bg){
	bool bFlg, sFlg, flg;
	int i, j, counter;
	unsigned short addr1, addr2;
	char *table1, *table2, temp;
	int line1, line2 , currentLine;
	unsigned short fineX;
	int spriteSize;
	int spriteZero = 100;

	if((ctrRegister2 & 0x18) == 0){
		return;
	}

	counter = 0;
	/*背景のパレット選択*/
	bFlg = CheckBit(ctrRegister1, 4);
	
	/*中途半端に始まる最初のブロックの処理*/
	fineX = addr.x & 0x0007;
	
	addr1 = PrePattern(addr.v, bFlg);
	table1 = MemoryMap(addr1);
	table2 = table1 + 8 * sizeof(char);
	addr2 = PreConvColor(addr.v);
	
	while(fineX < 8){
		flg = CheckBit(*table1, 7 - fineX);
		temp = flg * 0b01;
		flg = CheckBit(*table2, 7 - fineX);
		temp += flg * 0b10;
		if (temp != 0){
			temp += addr2;
		}
		bg[counter] = temp;
		++counter ;
		++fineX;
	}
	
	if (CheckBit(ctrRegister1, 5)){
		spriteSize = 16;
	}else{
		spriteSize = 8;
	}
	
	/*スプライト0の処理*/
	if (C2I(spriteTable[0]) == lineCounter - 1){
//					ioPort->ppuIO[0x0002] |= 0b01000000;
		if ((ctrRegister2 & 0x18) == 0x18){
			sprite0Pattern = &spriteBuffer[bufferNum].pattern[0][0];
			sprite0X = &spriteBuffer[bufferNum].x;
			sprite0Y = spriteSize;
		}
	}
	
	/*2ブロック目以降の処理*/
	for (i = 0; i < 32; ++i){
		/*X方向のネームテーブルを選択*/
		if((addr.v & 0x001f) == 0x1f){
			addr.v ^= 0x041f;
		}else{
			++addr.v;
		}
		
		addr1 = PrePattern(addr.v, bFlg);
		table1 = MemoryMap(addr1);
		table2 = table1 + 8 * sizeof(char);
		addr2 = PreConvColor(addr.v);
		
		if (spriteSize == 8){
			/*スプライトデータを捜索*/
			SpriteImg8(i, lineCounter - 1 );
		}else{
			SpriteImg16(i, lineCounter - 1);
		}

		for (j = 0; j < 8; ++j){
			flg = CheckBit(*table1, 7 - j);
			temp = flg * 0b01;
			flg = CheckBit(*table2, 7 - j);
			temp += flg * 0b10;
			if (temp != 0){
				temp += addr2;
			}
			bg[counter] = temp;

			++counter;
		}
	}
	
	/*スプライト0の処理*/
	if (sprite0Y >= 0){
		bool sZeroFlg = false;
		for (int k = 0; k < 8; ++k){
			sZeroFlg |= ((sprite0Pattern[k + ((spriteSize - sprite0Y) << 3)] & 0b11) != 0) & ((bg[*sprite0X + k] & 0b11) != 0);
		}
		if (sZeroFlg){
			ioPort->ppuIO[0x0002] |= 0b01000000;
			sprite0Y = -1;
		}else{
			--sprite0Y;
		}
	}
	
	/*必要に応じてスプライトデータを上書き*/
	for (int k =0; k < 8; ++k){
		if (spriteBuffer[k].counter < spriteSize){
			if (spriteBuffer[k].bgFlg){
				for (int l = 0; l < 8; ++l){
					if ((bg[spriteBuffer[k].x + l] & 0b11) == 0){
						temp = spriteBuffer[k].pattern[spriteBuffer[k].counter][l];
						if ((temp & 0b11) != 0){
							bg[spriteBuffer[k].x + l] = temp;
						}
					}
				}
			}else{
				for (int l = 0; l < 8; ++l){
					temp = spriteBuffer[k].pattern[spriteBuffer[k].counter][l];
					if ((temp & 0b11) != 0){
						bg[spriteBuffer[k].x + l] = temp;
					}
				}
			}
			++spriteBuffer[k].counter;
		}
	}
	lineCounter += 1;
	
	if((addr.v & 0x7000) != 0x7000){
		addr.v += 0x1000;
	}else if((addr.v & 0x73e0) == 0x73a0){
		addr.v ^= 0x7ba0;
	}else if((addr.v & 0x73e0) == 0x73e0){
		addr.v &= 0x0c1f;
	}else{
		addr.v += 0x0020;
		addr.v &= 0x0fff;
	}

	addr.v &= 0x7be0;
	addr.v |= (addr.t & 0x041f);

}

/*スプライトイメージをバッファに生成*/
void Ppu::SpriteImg8(int x, int y){
	int flg;
	int sflg;
	int addr;
	int hInit, hPos, hChange, vPos, vChange;
	char *table1, *table2, temp, colorPalette;
	bool hRev, vRev;
	
	/*スプライトのデープル選択*/
	sflg = static_cast<int>(CheckBit(ctrRegister1, 3)) << 12;
	y -= 1;
	x <<= 3;
	
	for (int i = 0; i < 2; ++i){
		
		/*ライン上にスプライトが見つかった場合*/
		if (C2I(spriteTable[x]) == y){
			
			/*スプライトのオーバフローを確認*/
		if (spriteBuffer[bufferNum].counter == 0){
			ioPort->ppuIO[0x0002] |= 0b00100000;
			return;
		}

		ioPort->ppuIO[0x0002] &= 0b11011111;
			
			/*0ライン目にセット*/
			spriteBuffer[bufferNum].counter = 0;
			x += 1;
			
			/*スプライトパターンのアドレスを取得*/
			addr = C2I(spriteTable[x]) << 4;
			addr += sflg;
			
			x += 1;
			
			/*背景優先かどうかを判定*/
			spriteBuffer[bufferNum].bgFlg = CheckBit(spriteTable[x], 5);
			/*水平反転かどうかを判定*/
			hRev = CheckBit(spriteTable[x], 6);
			/*垂直反転かどうかを判定*/
			vRev = CheckBit(spriteTable[x], 7);
			/*カラーパレットの上位2ビットを取得*/
			colorPalette = ((spriteTable[x] << 2) & 0x0f) | 0x10 ;
			x += 1;
			
			/*スプライトのx座標の位置*/
			spriteBuffer[bufferNum].x = C2I(spriteTable[x]);
			x += 1;
			
			/*反転の処理*/
			if (hRev){
				hInit = 0;
				hChange = 1;
			}else{
				hInit = 7;
				hChange = -1;
			}
			if (vRev){
				vPos = addr + 7;
				vChange = -1;
			}else{
				vPos = addr;
				vChange = 1;
			}
			
			/*スプライトデータをバッファに展開*/
			for (int j = 0; j < 8; ++j){
				table1 = MemoryMap(vPos);
				table2 = table1 + 8 * sizeof(char);
				vPos += vChange;
				hPos = hInit;
				for (int k = 0; k < 8; ++k){
					flg = CheckBit(*table1, hPos);
					temp = flg * 0b01;
					flg = CheckBit(*table2, hPos);
					temp += flg * 0b10;
					hPos += hChange;
					if (temp != 0){
						temp += colorPalette;
					}
					spriteBuffer[bufferNum].pattern[j][k] = temp;
				}
			}
			bufferNum += 1;
			bufferNum &= 0b111;
		}else{
			x += 4;
		}
	}
}


/*スプライトイメージをバッファに生成*/
void Ppu::SpriteImg16(int x, int y){
	int flg;
	int sflg;
	int addr;
	int hInit, hPos, hChange, vPos, vChange;
	char *table1, *table2, temp, colorPalette;
	bool hRev, vRev;
	
	/*スプライトのデープル選択
	sflg = static_cast<int>(CheckBit(ctrRegister1, 3)) << 12;*/
	y -= 1;
	x <<= 3;
	
	for (int i = 0; i < 2; ++i){
		
		/*ライン上にスプライトが見つかった場合*/
		if (C2I(spriteTable[x]) == y){
			
			/*スプライトのオーバフローを確認*/
		if (spriteBuffer[bufferNum].counter == 0){
			ioPort->ppuIO[0x0002] |= 0b00100000;
			return;
		}

		ioPort->ppuIO[0x0002] &= 0b11011111;
			
			/*0ライン目にセット*/
			spriteBuffer[bufferNum].counter = 0;
			x += 1;
			
			/*スプライトのデープル選択*/
			sflg = static_cast<int>(CheckBit(spriteTable[x], 0)) << 12;
			
			/*スプライトパターンのアドレスを取得*/			
			addr = (spriteTable[x] & 0x000000fe) << 4;
			addr += sflg;
			
			x += 1;
			
			/*背景優先かどうかを判定*/
			spriteBuffer[bufferNum].bgFlg = CheckBit(spriteTable[x], 5);
			/*水平反転かどうかを判定*/
			hRev = CheckBit(spriteTable[x], 6);
			/*垂直反転かどうかを判定*/
			vRev = CheckBit(spriteTable[x], 7);
			/*カラーパレットの上位2ビットを取得*/
			colorPalette = ((spriteTable[x] << 2) & 0x0f) | 0x10 ;
			x += 1;
			
			/*スプライトのx座標の位置*/
			spriteBuffer[bufferNum].x = C2I(spriteTable[x]);
			x += 1;
			
			/*反転の処理*/
			if (hRev){
				hInit = 0;
				hChange = 1;
			}else{
				hInit = 7;
				hChange = -1;
			}
			if (!vRev){
				vPos = addr;
				vChange = 1;
			}else{
				vPos = addr + 23;
				vChange = -1;
			}
			
			/*スプライトデータをバッファに展開*/
			for (int j = 0; j < 8; ++j){
				table1 = MemoryMap(vPos);
				table2 = table1 + 8 * sizeof(char);
				vPos += vChange;
				hPos = hInit;
				for (int k = 0; k < 8; ++k){
					flg = CheckBit(*table1, hPos);
					temp = flg * 0b01;
					flg = CheckBit(*table2, hPos);
					temp += flg * 0b10;
					hPos += hChange;
					if (temp != 0){
						temp += colorPalette;
					}
					spriteBuffer[bufferNum].pattern[j][k] = temp;
				}
			}
			vPos += (vChange << 3);
			/*スプライトデータをバッファに展開*/
			for (int j = 8; j < 16; ++j){
				table1 = MemoryMap(vPos);
				table2 = table1 + 8 * sizeof(char);
				vPos += vChange;
				hPos = hInit;
				for (int k = 0; k < 8; ++k){
					flg = CheckBit(*table1, hPos);
					temp = flg * 0b01;
					flg = CheckBit(*table2, hPos);
					temp += flg * 0b10;
					hPos += hChange;
					if (temp != 0){
						temp += colorPalette;
					}
					spriteBuffer[bufferNum].pattern[j][k] = temp;
				}
			}
			bufferNum += 1;
			bufferNum &= 0b111;
		}else{
			x += 4;
		}
	}
}
