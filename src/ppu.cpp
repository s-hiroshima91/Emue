/*ppuクラスのソース*/

#include "ppu.h"
#include "ioport.h"
#include "common.h"
#include <iostream>



/*場所からパレットを紐付ける関数*/
inline char PaletteNum(char value, int x, int y){
	int num;
	/*縦32lineで1単位なので
	下の二行で32で割って8かける操作*/
	num = y >> 2;
	num &= 0b0100;
	
	/*横4ブロックの前半か後半を判定*/
	num += (x & 0b10);
	/*塊4ブロックのどこかによって8ビットの
何番目を取り出すか判定する*/
	value >>= num;
	value <<= 2;
	/*パレットの上位2ビットを返す*/
	return value & 0b00001100;
}

/*場所からパターンテーブルを決定するクラス*/
int Ppu::PrePattern(int x, int y, int name, int line, bool flg){
	int addr;
	addr = C2I((ppuTable[x + y + name]));
	addr <<= 4;
	addr |= ((static_cast<int>(flg)) << 12); 	
	addr += (line & 0x07);
	return addr;
}

/*パレットの値に変換するクラス*/
char Ppu::PreConvColor(int x, int y, int name, int line){
	char value;
	
	value = ppuTable[y + (x >> 2) + name];
	value = PaletteNum(value, x, line);
	return value;
}

/*インストラクタ*/
Ppu::Ppu(char *romDate, char header6, IOPort *ioP){
	
	rom1 = romDate;
	rom2 = rom1 + 0x0800 * sizeof(char);
	rom3 = rom2 + 0x0800 * sizeof(char);
	rom4 = rom3 + 0x0800 * sizeof(char);
	
	vMirror = CheckBit(header6, 0);
	hMirror = !vMirror;
	ioPort = ioP;
	ioPort->ppuClass = this;
	lineCounter = 0x0000;
	ioPort->ppuIO[0x0002] = 0b10000000;
}

/*メモリマップ*/
char *Ppu::MemoryMap(unsigned short addr){
	char *pointer;
	if (addr < 0x0800){
		pointer = &rom1[addr];
		
	}else if (addr < 0x1000){
		addr -= 0x0800;
		pointer = &rom2[addr];
		
	}else if (addr < 0x1800){
		addr -= 0x1000;
		pointer = &rom3[addr];
		
	}else if (addr < 0x2000){
		addr -= 0x1800;
		pointer = &rom4[addr];
		
	}else if (addr < 0x3f00){
		addr -= 0x2000;
		addr %= 0x1000;
		pointer = &ppuTable[addr];
		
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
	int addr1, addr2;
	char *table1, *table2, temp;
	int nameAddr, nameAddrX, nameAddrY;
	int line1, line2 , currentLine;
	int x, y;
	int spriteSize;

	counter = 0;
	/*背景のパレット選択*/
	bFlg = CheckBit(ctrRegister1, 4);
	
	/*ネームテーブルの2ビット目を取得*/
	nameAddrY = (static_cast<int>(ctrRegister1) & 0b10) << 10;
	
	/*Yオフセット*/
	y = C2I(scroll[1]);
	if (y < 240){
		currentLine = lineCounter + C2I(scroll[1]);
	}else{
		currentLine = lineCounter + 240 + 240 - 256 + C2I(scroll[1]);
	}
	
	/*Y方向のネームテーブルを選択*/
	while (currentLine >= 240){
		nameAddrY ^= 0b100000000000;
		currentLine -= 240;
	}
	/*垂直ミラーの場合の処理*/
	if (vMirror){
		nameAddrY = name;
	}
	
	//line / 8 * 32
	line1 = ((currentLine << 2) & 0xffe0);
	
	//line / 32 * 8
	line2 = ((currentLine >> 2) & 0x38) + 0x03c0;
	
/*スプライト0の処理。未実装*/
	if (C2I(spriteTable[0]) == lineCounter + 1){
		ioPort->ppuIO[0x0002] |= 0b01000000;
	}else{
		ioPort->ppuIO[0x0002] &= 0b10111111;
	}
	
	/*ネームテーブルの1ビット目を取得*/
	nameAddrX = (static_cast<int>(ctrRegister1) & 0b01) << 10;
	
	/*ネームテーブルの選択*/
	nameAddr = nameAddrX | nameAddrY;
	
	/*中途半端に始まる最初のブロックの処理*/
	x = C2I(scroll[0]) >> 3;
	j = C2I(scroll[0]) & 0b0111;
	
	addr1 = PrePattern(x, line1, nameAddr, currentLine, bFlg);
	table1 = MemoryMap(addr1);
	table2 = table1 + 8 * sizeof(char);
	addr2 = PreConvColor(x, line2, nameAddr, currentLine);
	
	while(j < 8){
		flg = CheckBit(*table1, 7 - j);
		temp = flg * 0b01;
		flg = CheckBit(*table2, 7 - j);
		temp += flg * 0b10;
		if (temp != 0){
			temp += addr2;
		}
		bg[counter] = temp;
		++counter ;
		++j;
	}
	x += 1;
	

	if (CheckBit(ctrRegister1, 5)){
		spriteSize = 16;
	}else{
		spriteSize = 8;
	}
	
	
	/*2ブロック目以降の処理*/
	for (i = 0; i < 32; ++i){
		/*X方向のネームテーブルを選択*/
		if(x >= 32){
			x -= 32;
			nameAddrX ^= 0b010000000000;
		}
		/*水平ミラーの場合の処理*/
		if (hMirror){
			nameAddrX = name;
		}
		
		nameAddr = nameAddrX | nameAddrY;
		
		addr1 = PrePattern(x, line1, nameAddr, currentLine, bFlg);
		table1 = MemoryMap(addr1);
		table2 = table1 + 8 * sizeof(char);
		addr2 = PreConvColor(x, line2, nameAddr, currentLine);
		
		if (spriteSize == 8){
			/*スプライトデータを捜索*/
			SpriteImg8(i, lineCounter);
		}else{
			SpriteImg16(i, lineCounter);
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

			++counter ;
		}
		++x;
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
