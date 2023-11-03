/*ppuクラスのソース*/

#include "ppu.hpp"
#include "mapper.hpp"
#include <iostream>


/*場所からパレットを紐付ける関数*/
inline uint8_t PaletteNum(uint8_t value, uint16_t addr){
	/*縦横32lineの塊を1単位として
	パレットを選択する*/
	/*valueにはパレットの値が渡される。*/
	/*addrにはaddr.vが渡される。*/
	uint16_t num;
	/*縦32lineの前半か後半かを判定
	後半の場合、3bit目を立てる*/
	num = addr >> 4;
	num &= 0x0004;
	
	/*横32lineの前半か後半を判定
	後半の場合、2bit目を立てる*/
	num += (addr & 0x0002);
	/*塊4ブロックのどこかによってパレットの
何番目を取り出すか判定する*/
	value >>= num;
	value <<= 2;
	/*パレットの上位2ビットを返す*/
	return value & 0b00001100;
}

/*場所からパターンテーブルを決定する関数*/
uint16_t Ppu::PrePattern(uint16_t vAddr, uint16_t flg){
	uint16_t value;
	/*ネームテーブルは0x2000～0x3000まで*/
	//valueの12bitで表現される
	value = 0x2000 | (vAddr & 0x0fff);
	//ネームの値を取得
	value = static_cast<uint16_t>(*MemoryMap(value));
	/*パターンテーブルは0x0000～0x2000まで*/
	//16バイトで1つのブロックを描画
	value <<= 4;
	//0x0000を使うか、0x1000を使うか
	value |= flg;
	//y座標系の8lineを反映する
	value += ((vAddr >> 12) & 0x0007);
	return value;
}

/*パレットの値に変換する関数*/
uint8_t Ppu::PreConvColor(uint16_t vAddr){
	uint16_t addr;
	uint8_t value;
	
	addr = 0x23c0 | (vAddr & 0x0c00);
	addr |= ((vAddr >> 4) & 0x0038);
	addr |= ((vAddr >> 2) & 0x0007);
	
	value = *MemoryMap(addr);
	value = PaletteNum(value, vAddr);
	return value;
}

/*コンストラクタ*/
Ppu::Ppu(Mapper *MAPPER){

	mapper = MAPPER;
	lineCounter = 0x0000;
	ppuStatus = 0b10000000;
	if (mapper->mapperNum == 4){
		A12 = &Ppu::Mapper004;
	}else{
		A12 = &Ppu::Dummy;
	}
}

/*メモリマップ*/
uint8_t *Ppu::MemoryMap(uint16_t addr){
	uint8_t *pointer;
	addr &= 0x3fff;

	if (addr < 0x2000){
		//マッパーのメモリマップに従う
		//11～13bitでppuMapを選ぶ
		pointer = mapper->ppuMap[addr >> 10];
		//1～10bitで値を取得する
		pointer += addr & 0x3ff;
		
/*	}else if (addr < 0x3f00){
		addr -= 0x2000;
		addr %= 0x1000;
		pointer = &ppuTable[addr];*/
		
	}else if (addr < 0x3f00){
		addr &= 0x0fff;
		//マッパーのメモリマップに従う
		//11,12bitでscreenを選択する
		pointer = mapper->screen[addr >> 10];
		//1～10bitで値を取得する
		pointer += addr & 0x3ff;
		//2000～3f00はミラー
		
	}else if (addr < 0x4000){
		addr &= 0x1f;
		pointer = &ppuPalette[addr];
		
	}else{
		std::cout << "ppuerror 0x" << std::hex << addr << std::endl;
		pointer = nullptr;
	}
	return pointer;
}

/*1line分のデータを配列に格納するクラス*/
void Ppu::CreateImg(uint8_t *bg){
	int i, j, k, counter;
	uint16_t addr1, addr2;
	uint8_t *table1, *table2, temp, flg;
	uint16_t fineX;

	counter = 0;
	
	if (!CheckBit(ppuMask, 4)){
		spriteNum = 0;
	}
	
	/*背景のパレット選択*/
	a12 = (this->*A12)(static_cast<uint16_t>(ppuCtr & 0x10) << (12 - 4));
	//std::cout<<std::hex<<"a12 " << a12 <<std::endl;
	if (CheckBit(ppuMask, 3)){
		/*中途半端に始まる最初のブロックの処理*/
		fineX = addr.x & 0x0007;
		
		if (!CheckBit(ppuMask, 1)){
			for (counter = 0; counter < 8; ++counter){
				bg[counter] = 0;
			}
			if((addr.v & 0x001f) == 0x1f){
			addr.v ^= 0x041f;
			}else{
				++addr.v;
			}
		}
	
		addr1 = PrePattern(addr.v, a12);
		table1 = MemoryMap(addr1);
		table2 = table1 + 8;
		addr2 = PreConvColor(addr.v);
	
		while(fineX < 8){
			flg = CheckBitFF(*table1, 7 - fineX);
			temp = flg & 0b01;
			flg = CheckBitFF(*table2, 7 - fineX);
			temp += flg & 0b10;
			if (temp != 0){
				temp += addr2;
			}
			bg[counter] = temp;
			++counter;
			++fineX;
		}

		/*2ブロック目以降の処理*/
		while (counter < winX){
			/*X方向のネームテーブルを選択*/
			if((addr.v & 0x001f) == 0x1f){
			addr.v ^= 0x041f;
			}else{
				++addr.v;
			}
		
			addr1 = PrePattern(addr.v, a12);
			table1 = MemoryMap(addr1);
			table2 = table1 + 8;
			addr2 = PreConvColor(addr.v);

			for (j = 0; j < 8; ++j){
				flg = CheckBitFF(*table1, 7 - j);
				temp = flg & 0b01;
				flg = CheckBitFF(*table2, 7 - j);
				temp += flg & 0b10;
				if (temp != 0){
					temp += addr2;
				}
				bg[counter] = temp;

				++counter;
			}
		}
	}else{
		for (i = 0; i < winX; ++i){
			bg[i] = 0;
		}
	}
	
	/*スプライト0の処理*/
	
	if (s0Flg < spriteSize){
		s0Flg = lineCounter - static_cast<int>(spriteTable[0]);
		
		if ((s0Flg > 0) && (s0Flg <= spriteSize)){
			if ((ppuMask & 0x18) == 0x18){
				bool hit = false;
				k = 0;
				if (!CheckBit(ppuMask, 2)){
					k = 8 - sprite[0].x;
					if (k < 0){
						k = 0;
					}
				}
				int finish = 8;
				if (sprite[0].x > (winX - 9)){
					finish = winX - 1 - sprite[0].x;
				}
			
				while (k < finish){
					hit |= (((sprite[0].pattern[k] & 0b11) != 0) && ((bg[sprite[0].x + k] & 0b11) != 0));
					++k;
				}
				if (hit){
					ppuStatus |= 0b01000000;
				}
			}
		}
	}
	
	/*必要に応じてスプライトデータを上書き*/
	k = 0;	
	while (k < spriteNum){
		if (sprite[k].bgFlg){
			for (int l = 0; l < 8; ++l){
				if ((bg[sprite[k].x + l] & 0b11) == 0){
					temp = sprite[k].pattern[l];
					if ((temp & 0b11) != 0){
						bg[sprite[k].x + l] = temp;
					}
				}
			}
		}else{
			for (int l = 0; l < 8; ++l){
				temp = sprite[k].pattern[l];
				if ((temp & 0b11) != 0){
					bg[sprite[k].x + l] = temp;
				}
			}
		}
		++k;
	}
	
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
void Ppu::SpriteImg(){
	uint8_t flg;
	uint16_t addr;
	int hPos, hChange, temp;
	uint8_t *table1, *table2, img, colorPalette;
	int i = 0;

	/*スプライトのテーブル選択*/
	if (CheckBit(ppuCtr, 5)){
		spriteSize = 16;
	}else{
		spriteSize = 8;
		a12 = (this->*A12)(static_cast<uint16_t>(ppuCtr & 0x08) << (12 - 3));
	}

	spriteNum = 0;
	while (i < 0x100){
		temp = lineCounter - static_cast<int>(spriteTable[i]);
		/*ライン上にスプライトが見つかった場合*/
		if ((temp >= 0) && (temp < spriteSize)){
			++i;
							
			/*スプライトパターンのアドレスを取得*/
			addr = static_cast<uint16_t>(spriteTable[i]) << 4;

			if (spriteSize == 16){
				/*スプライトテーブル選択*/
				a12 = (this->*A12)(static_cast<uint16_t>(spriteTable[i] & 0x01) << 12);
				addr &= 0xffe0;
			}
			addr += a12;
			++i;
			
			/*垂直反転*/
			if (CheckBit(spriteTable[i], 7)){
				addr += (spriteSize & 0x10) + 7 - temp;
				addr -= (temp & 0x08);
			}else{
				addr += temp;
				addr += (temp & 0x08);
			}
			/*水平反転*/
			if (CheckBit(spriteTable[i], 6)){
				hPos = 0;
				hChange = 1;
			}else{
				hPos = 7;
				hChange = -1;
			}

			/*背景優先かどうかを判定*/
			sprite[spriteNum].bgFlg = CheckBit(spriteTable[i], 5);
			/*	カラーパレットの上位2ビットを取得*/
			colorPalette = ((spriteTable[i] << 2) & 0x0f) | 0x10 ;
			++i;

			table1 = MemoryMap(addr);
			table2 = table1 + 8;
			for (int j = 0; j < 8; ++j){
				flg = CheckBitFF(*table1, hPos);
				img = flg & 0b01;
				flg = CheckBitFF(*table2, hPos);
				img += flg & 0b10;
				hPos += hChange;
				if (img != 0){
					img += colorPalette;
				}
				sprite[spriteNum].pattern[j] = img;
			}
			/*スプライトのx座標の位置*/
			sprite[spriteNum].x = static_cast<int>(spriteTable[i]);
			++i;
			++spriteNum;
			if (spriteNum == 8){
				i |= 0x100;
			}
		}else{
			i += 4;
		}
	}
	
	if (i != 0x100){
		i &= 0xff;
	}
	
	if(spriteSize == 16){
		if(spriteNum != 8){
			a12 = (this->*A12)(0x1000);
		}
	}
	
	/*オーバーフローの捜索*/
	int fine = 0;
	while(i < 0x100){
		temp = lineCounter - static_cast<int>(spriteTable[i + fine]);
		/*ライン上にスプライトが見つかった場合*/
		if ((temp >= 0) && (temp < 16)){
			/*オーバフローフラグを立てる*/
			ppuStatus |= 0b00100000;
			i = 0x100;
		}else{
			++fine;
			fine &= 0x03;
			i += 4;
		}
	}
}

uint16_t Ppu::Mapper004(uint16_t after){
	uint16_t temp;
	temp = after & 0x1000;
	if ((temp & (0x1000 ^ a12)) != 0){
		mapper->MapperIRQ();
		//std::cout << std::hex << "irq " << mapper->irqCounter << std::endl;
	}
	a12 = temp;
	return after;
}

uint16_t Ppu::Dummy(uint16_t after){
	return after;
}