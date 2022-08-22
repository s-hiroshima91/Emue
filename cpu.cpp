#include <iostream>
#include "cpu.h"
#include "ioport.h"
#include "common.h"

//ゼロを確認するための関数
bool CheckZero(char operand){
	bool flg = false;
	if (operand == 0){
		flg = true;
	}
	return flg;
}

//16bitアドレスに変換
unsigned short C2US(char value){
	return value & 0x00ff; //マイナスの扱いを明確にするため上8桁をマスク
}

//16bitアドレスに変換、ただし符号を考慮
unsigned short C2S(char valueC){
	bool flg;
	unsigned short valueS;
	//最上位bitが1なら追加する桁を1で埋める
	flg = CheckBit(valueC, 7);
	if (flg == true){
		valueS = valueC | 0xff00;
	}else{
		valueS = valueC & 0x00ff;
	}
	return valueS;
}
	
//2バイト読み込む関数
unsigned short PrgCounter(char *addr){
	return (C2US(addr[1]) << 8) + C2US(addr[0]);
}

//ステータスレジスタのbit操作
void Cpu::StatusRegi(int sflg, bool status){
	char flg = 0x01;
		flg <<= sflg;
	if (status == true){
		registerP |= flg;
	}else{
		flg ^= 0xff;
		registerP &= flg;
	}
}

//以下、cpuの命令

/*対象の値をレジスタに入力*/
void Cpu::LdO(char *wRegister, char operand){
	bool status;
	*wRegister = operand;
	status = CheckZero(*wRegister);
	StatusRegi(zFlg, status);
	status = CheckBit(*wRegister, 7);
	StatusRegi(nFlg, status);
}

/*レジスタの値を対象に入力*/
void Cpu::StO(unsigned short wAddr, char rRegister){
		char *ptr;
		ptr = MemoryMap(wAddr);
		*ptr = rRegister;
}

/*対象の値をレジスタAに加算する*/
void Cpu::Adc(char *wRegister, char operand){
	unsigned short calc;
	bool flgA, flgB, cStatu, zStatu,vStatu, nStatu;
	/*キャリーオーバを防ぐためshortで計算する*/
	calc = C2US(*wRegister) + C2US(operand);
	cStatu = CheckBit(registerP, cFlg);
	calc += static_cast<unsigned short>(cStatu);
	cStatu = static_cast<bool>(calc >> 8);
	flgA = CheckBit(*wRegister, 7);
	flgB = CheckBit(operand, 7);
	/*念のため上位バイトを0で埋めてからcharに変換*/
	*wRegister = static_cast<char>(calc & 0x00ff);
	nStatu = CheckBit(*wRegister, 7);
	vStatu = flgA & flgB & (! nStatu);
	vStatu |= ((! flgA) & (! flgB) & nStatu);
	zStatu = CheckZero(*wRegister);
	StatusRegi(cFlg, cStatu);
	StatusRegi(zFlg, zStatu);
	StatusRegi(vFlg, vStatu);
	StatusRegi(nFlg, nStatu);
}

/*対象の値をレジスタAから引く*/
void Cpu::Sbc(char *wRegister, unsigned short addr){
	char value;
	/*わたす値を2の補数にすることで足し算と共通の計算とする。*/
	value = *MemoryMap(addr);
	value ^= 0xff; //2の補数計算
	//operand++;  ここで1を足さないことで足し算とキャリーフラグの扱いを共通化
	Adc(wRegister, value);
}

/*対象の値とレジスタAのAnd*/
void Cpu::And(unsigned short addr){
	char value;
	bool status; 
	registerA &= *MemoryMap(addr);
	status = CheckZero(registerA);
	StatusRegi(zFlg, status);
	status = CheckBit(registerA, 7);
	StatusRegi(nFlg, status);
}

/*対象の値とレジスタAのOr*/
void Cpu::Ora(unsigned short addr){
	char value;
	bool status;
	registerA |= *MemoryMap(addr);
	status = CheckZero(registerA);
	StatusRegi(zFlg, status);
	status = CheckBit(registerA, 7);
	StatusRegi(nFlg, status);
}

/*対象の値とレジスタAのXor*/
void Cpu::Eor(unsigned short addr){
	char value;
	bool status;
	registerA ^= *MemoryMap(addr);
	status = CheckZero(registerA);
	StatusRegi(zFlg, status);
	status = CheckBit(registerA, 7);
	StatusRegi(nFlg, status);
}

/*対象の値を左シフト*/
void Cpu::AslA(char *operand){
	bool status;
	status = CheckBit(*operand, 7);
	StatusRegi(cFlg, status);
	status = CheckBit(*operand, 6);
	StatusRegi(nFlg, status);
	*operand <<= 1;
	status = CheckZero(*operand);
	StatusRegi(zFlg, status);
}

/*対象の値を左シフト*/
void Cpu::Asl(unsigned short addr){
	char *ptr;
	ptr = MemoryMap(addr);
	AslA(ptr);
}

/*対象の値を右シフト*/
void Cpu::LsrA(char *operand){
	bool status;
	status = CheckBit(*operand, 0);
	StatusRegi(cFlg, status);
	StatusRegi(nFlg, false);
	*operand >>= 1;
	/*最上位bitに0を入力*/
	*operand &= 0b01111111;
	status = CheckZero(*operand);
	StatusRegi(zFlg, status);
}

/*対象の値を右シフト*/
void Cpu::Lsr(unsigned short addr){
	char *ptr;
	ptr = MemoryMap(addr);
	LsrA(ptr);
}

/*対象の値をキャリー含めて左シフト*/
void Cpu::RolA(char *operand){
	bool status;
	char  temp;
	status = CheckBit(registerP, cFlg);
	temp = static_cast<char>(status);
	status = CheckBit(*operand, 7);
	StatusRegi(cFlg, status);
	status = CheckBit(*operand, 6);
	StatusRegi(nFlg, status);
	*operand <<= 1;
	*operand += temp;
	status = CheckZero(*operand);
	StatusRegi(zFlg, status);
}

/*対象の値をキャリー含めて左シフト*/
void Cpu::Rol(unsigned short addr){
	char *ptr;
	ptr = MemoryMap(addr);
	RolA(ptr);
}

/*対象の値をキャリー含めて右シフト*/
void Cpu::RorA(char *operand){
	bool status;
	char temp;
	status = CheckBit(registerP, cFlg);
	temp = static_cast<char>(status);
	temp <<= 7;
	StatusRegi(nFlg, status);
	status = CheckBit(*operand, 0);
	StatusRegi(cFlg, status);
	*operand >>= 1;
	*operand &= 0b01111111;
	*operand += temp;
	status = CheckZero(*operand);
	StatusRegi(zFlg, status);
}

/*対象の値をキャリー含めて右シフト*/
void Cpu::Ror(unsigned short addr){
	char *ptr;
	ptr = MemoryMap(addr);
	RorA(ptr);
}

/*対象のbitを確認*/
void Cpu::Bit(char operand){
	bool status;
	status = CheckBit(operand, 7);
	StatusRegi(nFlg, status);
	status = CheckBit(operand, 6);
	StatusRegi(vFlg, status);
	operand &= registerA;
	status = CheckZero(operand);
	StatusRegi(zFlg, status);
}

/*対象の値と対象のレジスタの値を比較*/
void Cpu::CmO(char rRegister, unsigned short rMemory){
	char temp;
	bool Vtemp;
	temp = rRegister; //レジスタの値を書き換えない
	Vtemp = CheckBit(registerP, vFlg);
	StatusRegi(cFlg, true); //ボローなし引き算のためキャリーをセット
	Sbc(&temp, rMemory);
	StatusRegi(vFlg, Vtemp);
}

/*対象の値をインクリメント*/
void Cpu::InO(char *addr){
	bool status;
	*addr += 1;
	status = CheckZero(*addr);
	StatusRegi(zFlg, status);
	status = CheckBit(*addr, 7);
	StatusRegi(nFlg, status);
}

/*対象の値をインクリメント*/
void Cpu::Inc(unsigned short addr){
	char *ptr;
	ptr = MemoryMap(addr);
	InO(ptr);
}

/*対象の値をデクリメント*/
void Cpu::DeO(char *addr){
	bool status;
	*addr -= 1;
	status = CheckBit(*addr, 7);
	StatusRegi(nFlg, status);
	status = CheckZero(*addr);
	StatusRegi(zFlg, status);
}

/*対象の値をデクリメント*/
void Cpu::Dec(unsigned short addr){
	char *ptr;
	ptr = MemoryMap(addr);
	DeO(ptr);
}

/*レジスタの値をスタック*/
void Cpu::PhO(char rRegister){
	unsigned short addr;
	char *ptr;
	/*スタックのアドレスに変換*/
	addr = C2US(registerS) | 0x0100;
	ptr = MemoryMap(addr);
	*ptr = rRegister;
	registerS -= 1; //スタックポインタをデクリメント
}

/*スタックをレジスタの値に入力*/
void Cpu::PlO(char *wRegister){
	unsigned short addr;
	bool status;
	char value;
	//スタックポインタをインクリメント
	registerS += 1;
	addr = C2US(registerS) | 0x0100;
	value = *MemoryMap(addr);
	status = CheckBit(value, 7);
	StatusRegi(nFlg,status);
	status = CheckZero(value);
	StatusRegi(zFlg, status);
	*wRegister = value;
}

/*ジャンプ*/
void Cpu::Jmp(unsigned short addr){
	registerPC = addr;
}

/*サブルーチン呼び出し*/
void Cpu::Jsr(unsigned short addr){
	char value;
	registerPC -= 1;
	/*プログラムカウンタの値をスタック*/
	value = static_cast<char>(0x00ff & (registerPC >> 8));
	PhO(value);
	value = static_cast<char>(0x00ff & registerPC);
	PhO(value);
	registerPC = addr;
}
/*サブルーチンからの帰還*/
void Cpu::Rts(){
	char value;
	char temp;
	temp = registerP;
	/*プログラムカウンタの値を復帰*/
	PlO(&value);
	registerPC = C2US(value);
	PlO(&value);
	registerPC |= (C2US(value) << 8);
	registerP = temp;
	registerPC += 1;
}

/*割り込みからの復帰*/
void Cpu::Rti(){
	PlO(&registerP);
	Rts();
	registerPC -= 1;
}

/*ステータスレジスタのビットによる分岐*/
int Cpu::Bxx(int flg, bool status){
	bool temp;
	char value;
	temp = CheckBit(registerP, flg);
	if (temp == status){
		value = *MemoryMap(registerPC);
		registerPC += C2S(value);
		registerPC += 1;
		return 1;
	}
	registerPC += 1;
	return 0;
}

/*ソフトウェア割り込み*/
void Cpu::Brk(unsigned short addr){
	registerPC += 1;
	Jsr(addr);
	PhO(registerP);
	StatusRegi(iFlg, true);
	registerPC = PrgCounter(MemoryMap(addr));
}

/*スプライトデータ転送*/
int Cpu::DMA(){
	unsigned short addr;
	char *ptr;
	if (ioPort->dma == false){
		return 0;
}
	addr = C2US(ioPort->padIO[0x14]) << 8;
	ptr = MemoryMap(addr);
	ioPort->DMAFunc(ptr);
	return 513;
}


//以下、アドレッシング
void Cpu::Zpg(unsigned short *addr){
	char temp;
	temp = *MemoryMap(registerPC);
	registerPC += 1;
	*addr = C2US(temp);
}


void Cpu::ZpgO(char operand, unsigned short *addr){
	unsigned short temp;
	Zpg(&temp);
	temp += C2US(operand);
	*addr = temp & 0x00ff;
}

void Cpu::Abs(unsigned short *addr){
	unsigned short temp;
	Zpg(addr);
	Zpg(&temp);
	*addr = (temp << 8) + C2US(*addr);
}

int Cpu::AbsO(char operand, unsigned short *addr){
	unsigned short temp;
	Abs(&temp);
	*addr = temp + C2US(operand);
	if ((*addr & 0x0100) == (temp & 0x0100)){
		return 1;
	}
	return 0;
}
	
void Cpu::OInd(char operand, unsigned short *addr){
	unsigned short addr1;
	char addr2;
	Zpg(&addr1);
	addr1 = (addr1 + operand) & 0x00ff;
	addr2 = *MemoryMap((addr1 + 1) & 0x00ff);
	addr1 = C2US(*MemoryMap(addr1));
	*addr = addr1 + (C2US(addr2) << 8);
}

int Cpu::IndO(char operand, unsigned short *addr){
	unsigned short addr1;
	char addr2;
	Zpg(&addr1);
	addr2 = *MemoryMap((addr1 + 1) & 0x00ff);
	addr1 = C2US(*MemoryMap(addr1));
	addr1 += (C2US(addr2) << 8);
	*addr = addr1 + C2US(operand);
	if ((*addr & 0x0100) == (addr1 & 0x0100)){
		return 1;
	}
	return 0;
}

void Cpu::Ind(unsigned short *addr){
	unsigned short addr1;
	char addr2;
	Abs(&addr1);
	if ((addr1 & 0xff) == 0xff){
		addr2 = *MemoryMap(addr1- 0xff);
	}else{
		addr2 = *MemoryMap(addr1+ 1);
	}
	addr1 = C2US(*MemoryMap(addr1));
	*addr = addr1 + (C2US(addr2) << 8);
}

/*メモリマップ*/
char* Cpu::MemoryMap(unsigned short addr){
	char *pointer;
	if (addr < 0x2000){
		addr &= 0x07ff;
		pointer = &cpuRam[addr];
	}else if (addr < 0x4000){
		addr -= 0x2000;
		addr &= 0x0007;
		pointer = &ioPort->ppuIO[addr];
		ioPort->IOFlg(addr);
	}else if (addr < 0x4020){
		addr -= 0x4000;
		pointer = &ioPort->padIO[addr];
		ioPort->PadFlg(addr);
	}else if (addr < 0x6000){
		/*ext_rom 未実装*/
		
	}else if (addr < 8000){
		/*ext_ram 未実装*/

	}else{
		addr -= 0x8000;
		if (romSize == 1){
			addr &= 0x3fff;
		}
		pointer = &rom[addr];
	}
	return pointer;
}

/*割り込み*/
void Cpu::Interrupt(){
	bool flg;
	if (nmi == true){
		StatusRegi(bFlg, false);
		Brk(0xfffa);
		nmi = false;
	}else if (rst == true){
		registerPC = PrgCounter(MemoryMap(0xfffc));
		StatusRegi(iFlg, true);
		rst = false;
	}else if (irbr == true){
		flg = CheckBit(registerP, iFlg);
		if (flg == false){
			StatusRegi(bFlg, false);
			Brk(0xfffe);
		}
		irbr = false;
	}
}

/*インストラクタ*/
Cpu::Cpu(char *romDate, char header4, IOPort *ioP){
	nmi = false;
	rst = true;
	irbr = false;
	rom = romDate;
	ioPort = ioP;
	romSize = header4;
	registerP = 0b00100000;
	registerS = 0xfd;
	
	Interrupt();
	
}

/*メインのシーケンス*/
int Cpu::mainRun(){
	
	int counter;
	
	/*オペコードを取得*/
	opeCode = static_cast<enum opeCode>(*MemoryMap(registerPC) & 0x00ff);
	registerPC +=1;
	
	/*メインの処理実行*/
	counter = Operation();
	
	/*IOポートにアクセスがあった場合の処理*/
	ioPort->IOFunc();
	ioPort->PadFunc();
	counter += DMA();
	
	/*処理にかかったクロック数を返す*/
	return counter;
}

/*メインの処理*/
int Cpu::Operation(){
	int counter = 0;
	char tempC;
	char *ptr;
	unsigned short tempS;
	if (opeCode == LdaIm){ 
		tempC = *MemoryMap(registerPC);
		registerPC += 1;
		counter += cpuCycles[LdaIm];
		LdO(&registerA, tempC);
		
	}else if (opeCode == LdaZpg){
		Zpg(&tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdaZpg];
		LdO(&registerA, tempC);
		
	}else if (opeCode == LdaZpgX){
		ZpgO(registerX, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdaZpgX];
		LdO(&registerA, tempC);
		
	}else if (opeCode == LdaAbs){
		Abs(&tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdaAbs];
		LdO(&registerA, tempC);
		
	}else if (opeCode == LdaAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[LdaAbsX];
		tempC = *MemoryMap(tempS);
		LdO(&registerA, tempC);

	}else if (opeCode == LdaAbsY){
		counter += AbsO(registerY, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdaAbsY];
		LdO(&registerA, tempC);
		
	}else if (opeCode == LdaXInd){
		OInd(registerX, &tempS);
		counter += cpuCycles[LdaXInd];
		tempC = *MemoryMap(tempS);
		LdO(&registerA, tempC);
	
	}else if (opeCode == LdaIndY){
		counter += IndO(registerY, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdaIndY];
		LdO(&registerA, tempC);
		
	}else if (opeCode == LdxIm){ 
		tempC = *MemoryMap(registerPC);
		registerPC++;
		counter += cpuCycles[LdxIm] ;
		LdO(&registerX, tempC);
		
	}else if (opeCode == LdxZpg){
		Zpg(&tempS);
		counter += cpuCycles[LdxZpg];
		tempC = *MemoryMap(tempS);
		LdO(&registerX, tempC);
		
	}else if (opeCode == LdxZpgY){
		ZpgO(registerY, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdxZpgY];
		LdO(&registerX, tempC);
		
	}else if (opeCode == LdxAbs){
		Abs(&tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdxAbs];
		LdO(&registerX, tempC);
		
	}else if (opeCode == LdxAbsY){
		counter += AbsO(registerY, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdxAbsY];
		LdO(&registerX, tempC);

	}else if (opeCode == LdyIm){ 
		tempC = *MemoryMap(registerPC);
		registerPC += 1;
		counter += cpuCycles[LdyIm];
		LdO(&registerY, tempC);
		
	}else if (opeCode == LdyZpg){
		Zpg(&tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdyZpg];
		LdO(&registerY, tempC);
		
	}else if (opeCode == LdyZpgX){
		ZpgO(registerX, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdyZpgX];
		LdO(&registerY, tempC);
		
	}else if (opeCode == LdyAbs){
		Abs(&tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdyAbs];
		LdO(&registerY, tempC);
		
	}else if (opeCode == LdyAbsX){
		counter += AbsO(registerX, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[LdyAbsX];
		LdO(&registerY, tempC);

	}else if (opeCode == StaZpg){
		Zpg(&tempS);
		counter += cpuCycles[StaZpg];
		StO(tempS, registerA);
		
	}else if (opeCode == StaZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[StaZpgX];
		StO(tempS, registerA);

	}else if (opeCode == StaAbs){
		Abs(&tempS);
		counter += 2;
		StO(tempS, registerA);

	}else if (opeCode == StaAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[StaAbs];
		StO(tempS, registerA);
		
	}else if (opeCode == StaAbsY){
		counter += AbsO(registerY, &tempS);
		counter += cpuCycles[StaAbsX];
		StO(tempS, registerA);
		
	}else if (opeCode == StaXInd){
		OInd(registerX, &tempS);
		counter += cpuCycles[StaXInd];
		StO(tempS, registerA);
	
	}else if (opeCode == StaIndY){
		counter += IndO(registerY, &tempS);
		counter += cpuCycles[StaIndY];
		StO(tempS, registerA);
		
	}else if (opeCode == StxZpg){
		Zpg(&tempS);
		counter += cpuCycles[StxZpg];
		StO(tempS, registerX);
		
	}else if (opeCode == StxZpgY){
		ZpgO(registerY, &tempS);
		counter += cpuCycles[StxZpgY];
		StO(tempS, registerX);

	}else if (opeCode == StxAbs){
		Abs(&tempS);
		counter += cpuCycles[StxAbs];
		StO(tempS, registerX);

	}else if (opeCode == StyZpg){
		Zpg(&tempS);
		counter += cpuCycles[StyZpg];
		StO(tempS, registerY);
		
	}else if (opeCode == StyZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[StyZpgX];
		StO(tempS, registerY);

	}else if (opeCode == StyAbs){
		Abs(&tempS);
		counter += cpuCycles[StyAbs];
		StO(tempS, registerY);
		
	}else if (opeCode == TaxImp){
		LdO(&registerX, registerA);
		counter += cpuCycles[TaxImp];

	}else if (opeCode == TayImp){
		LdO(&registerY, registerA);
		counter += cpuCycles[TayImp];
		
	}else if (opeCode == TsxImp){
		LdO(&registerX, registerS);
		counter += cpuCycles[TsxImp];
		
	}else if (opeCode == TxaImp){
		LdO(&registerA, registerX);
		counter += cpuCycles[TxaImp];
		
	}else if (opeCode == TxsImp){
		registerS = registerX;
		counter += cpuCycles[TxsImp];
		
	}else if (opeCode == TyaImp){
		LdO(&registerA, registerY);
		counter += cpuCycles[TyaImp];
		
	}else if (opeCode == AdcIm){
		tempC = *MemoryMap(registerPC);
		counter += cpuCycles[AdcIm];
		Adc(&registerA, tempC);
		registerPC++;
		
	}else if (opeCode == AdcZpg){
		Zpg(&tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[AdcZpg];
		Adc(&registerA, tempC);
		
	}else if (opeCode == AdcZpgX){
		ZpgO(registerX, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[AdcZpgX];
		Adc(&registerA, tempC);
		
	}else if (opeCode == AdcAbs){
		Abs(&tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[AdcAbs];
		Adc(&registerA, tempC);
		
	}else if (opeCode == AdcAbsX){
		counter += AbsO(registerX, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[AdcAbsX];
		Adc(&registerA, tempC);

	}else if (opeCode == AdcAbsY){
		counter += AbsO(registerY, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[AdcAbsY];
		Adc(&registerA, tempC);
		
	}else if (opeCode == AdcXInd){
		OInd(registerX, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[AdcXInd];
		Adc(&registerA, tempC);
	
	}else if (opeCode == AdcIndY){
		counter += IndO(registerY, &tempS);
		tempC = *MemoryMap(tempS);
		counter += cpuCycles[AdcIndY];
		Adc(&registerA, tempC);
		
	}else if (opeCode == SbcIm){
		counter += cpuCycles[SbcIm];
		Sbc(&registerA, registerPC);
		registerPC += 1;
		
	}else if (opeCode == SbcZpg){
		Zpg(&tempS);
		counter += cpuCycles[SbcZpg];
		Sbc(&registerA, tempS);
		
	}else if (opeCode == SbcZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[SbcZpgX];
		Sbc(&registerA, tempS);
		
	}else if (opeCode == SbcAbs){
		Abs(&tempS);
		counter += cpuCycles[SbcAbs];
		Sbc(&registerA, tempS);
		
	}else if (opeCode == SbcAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[SbcAbsX];
		Sbc(&registerA, tempS);

	}else if (opeCode == SbcAbsY){
		counter += AbsO(registerY, &tempS);
		counter += cpuCycles[SbcAbsY];
		Sbc(&registerA, tempS);
		
	}else if (opeCode == SbcXInd){
		OInd(registerX, &tempS);
		counter += cpuCycles[SbcXInd];
		Sbc(&registerA, tempS);
	
	}else if (opeCode == SbcIndY){
		counter += IndO(registerY, &tempS);
		counter += cpuCycles[SbcIndY];
		Sbc(&registerA, tempS);
		
	}else if (opeCode == IncZpg){
		Zpg(&tempS);
		counter += cpuCycles[IncZpg];
		Inc(tempS);
		
	}else if (opeCode == IncZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[IncZpgX];
		Inc(tempS);

	}else if (opeCode == IncAbs){
		Abs(&tempS);
		counter += cpuCycles[IncAbs];
		Inc(tempS);

	}else if (opeCode == IncAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[IncAbsX];
		Inc(tempS);

	}else if (opeCode == InXImp){
		counter += cpuCycles[InXImp];
		InO(&registerX);

	}else if (opeCode == InYImp){
		counter += cpuCycles[InYImp];
		InO(&registerY);

	}else if (opeCode == DecZpg){
		Zpg(&tempS);
		counter += cpuCycles[DecZpg];
		Dec(tempS);
		
	}else if (opeCode == DecZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[DecZpgX];
		Dec(tempS);

	}else if (opeCode == DecAbs){
		Abs(&tempS);
		counter += cpuCycles[DecAbs];
		Dec(tempS);

	}else if (opeCode == DecAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[DecAbsX];
		Dec(tempS);

	}else if (opeCode == DeXImp){
		counter += cpuCycles[DeXImp];
		DeO(&registerX);

	}else if (opeCode == DeYImp){
		counter += cpuCycles[DeYImp];
		DeO(&registerY);


	}else if (opeCode == AndIm){
		counter += cpuCycles[AndIm];
		And(registerPC);
		registerPC++;
		
	}else if (opeCode == AndZpg){
		Zpg(&tempS);
		counter += cpuCycles[AndZpg];
		And(tempS);
		
	}else if (opeCode == AndZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[AndZpgX];
		And(tempS);
		
	}else if (opeCode == AndAbs){
		Abs(&tempS);
		counter += cpuCycles[AndAbs];
		And(tempS);
		
	}else if (opeCode == AndAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[AndAbsX];
		And(tempS);

	}else if (opeCode == AndAbsY){
		counter += AbsO(registerY, &tempS);
		counter += cpuCycles[AndAbsY];
		And(tempS);
		
	}else if (opeCode == AndXInd){
		OInd(registerX, &tempS);
		counter += cpuCycles[AndXInd];
		And(tempS);
	
	}else if (opeCode == AndIndY){
		counter += IndO(registerY, &tempS);
		counter += cpuCycles[AndIndY];
		And(tempS);
		
	}else if (opeCode == OraIm){
		counter += cpuCycles[OraIm];
		Ora(registerPC);
		registerPC += 1;
		
	}else if (opeCode == OraZpg){
		Zpg(&tempS);
		counter += cpuCycles[OraZpg];
		Ora(tempS);
		
	}else if (opeCode == OraZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[OraZpgX];
		Ora(tempS);
		
	}else if (opeCode == OraAbs){
		Abs(&tempS);
		counter += cpuCycles[OraAbs];
		Ora(tempS);
		
	}else if (opeCode == OraAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[OraAbsX];
		Ora(tempS);

	}else if (opeCode == OraAbsY){
		counter += AbsO(registerY, &tempS);
		counter += cpuCycles[OraAbsY];
		Ora(tempS);
		
	}else if (opeCode == OraXInd){
		OInd(registerX, &tempS);
		counter += cpuCycles[OraXInd];
		Ora(tempS);
	
	}else if (opeCode == OraIndY){
		counter += IndO(registerY, &tempS);
		counter += cpuCycles[OraIndY];
		Ora(tempS);
		
	}else if (opeCode == EorIm){
		counter += cpuCycles[EorIm];
		Eor(registerPC);
		registerPC+= 1;
		
	}else if (opeCode == EorZpg){
		Zpg(&tempS);
		counter += cpuCycles[EorZpg];
		Eor(tempS);
		
	}else if (opeCode == EorZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[EorZpgX];
		Eor(tempS);
		
	}else if (opeCode == EorAbs){
		Abs(&tempS);
		counter += cpuCycles[EorAbs];
		Eor(tempS);
		
	}else if (opeCode == EorAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[EorAbsX];
		Eor(tempS);

	}else if (opeCode == EorAbsY){
		counter += AbsO(registerY, &tempS);
		counter += cpuCycles[EorAbsY];
		Eor(tempS);
		
	}else if (opeCode == EorXInd){
		OInd(registerX, &tempS);
		counter += cpuCycles[EorXInd];
		Eor(tempS);
	
	}else if (opeCode == EorIndY){
		counter += IndO(registerY, &tempS);
		counter += cpuCycles[EorIndY];
		Eor(tempS);
		
	}else if(opeCode == AslAA){
		counter += cpuCycles[AslAA];
		AslA(&registerA);
		
	}else if (opeCode == AslZpg){
		Zpg(&tempS);
		counter += cpuCycles[AslZpg];
		Asl(tempS);
		
	}else if (opeCode == AslZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[AslZpgX];
		Asl(tempS);
		
	}else if (opeCode == AslAbs){
		Abs(&tempS);
		counter += cpuCycles[AslAbs];
		Asl(tempS);
		
	}else if (opeCode == AslAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[AslAbsX];
		Asl(tempS);

	}else if(opeCode == LsrAA){
		counter += cpuCycles[LsrAA];
		LsrA(&registerA);
		
	}else if (opeCode == LsrZpg){
		Zpg(&tempS);
		counter += cpuCycles[LsrZpg];
		Lsr(tempS);
		
	}else if (opeCode == LsrZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[LsrZpgX];
		Lsr(tempS);
		
	}else if (opeCode == LsrAbs){
		Abs(&tempS);
		counter += cpuCycles[LsrAbs];
		Lsr(tempS);
		
	}else if (opeCode == LsrAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[LsrAbsX];
		Lsr(tempS);

	}else if(opeCode == RolAA){
		counter += cpuCycles[RolAA];
		RolA(&registerA);
		
	}else if (opeCode == RolZpg){
		Zpg(&tempS);
		counter += cpuCycles[RolZpg];
		Rol(tempS);
		
	}else if (opeCode == RolZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[RolZpgX];
		Rol(tempS);
		
	}else if (opeCode == RolAbs){
		Abs(&tempS);
		counter += cpuCycles[RolAbs];
		Rol(tempS);
		
	}else if (opeCode == RolAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[RolAbsX];
		Rol(tempS);

	}else if(opeCode == RorAA){
		counter += cpuCycles[RorAA];
		RorA(&registerA);
		
	}else if (opeCode == RorZpg){
		Zpg(&tempS);
		counter += cpuCycles[RorZpg];
		Ror(tempS);
		
	}else if (opeCode == RorZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[RorZpgX];
		Ror(tempS);
		
	}else if (opeCode == RorAbs){
		Abs(&tempS);
		counter += cpuCycles[RorAbs];
		Ror(tempS);
		
	}else if (opeCode == RorAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[RorAbsX];
		Ror(tempS);

	}else if (opeCode == BitZpg){
		Zpg(&tempS);
		counter += cpuCycles[BitZpg];
		tempC = *MemoryMap(tempS);
		Bit(tempC);
			
	}else if (opeCode == BitAbs){
		Abs(&tempS);
		counter += cpuCycles[BitAbs];
		tempC = *MemoryMap(tempS);
		Bit(tempC);

	}else if (opeCode == CmpIm){
		counter += cpuCycles[CmpIm];
		CmO(registerA, registerPC);
		registerPC += 1;
		
	}else if (opeCode == CmpZpg){
		Zpg(&tempS);
		counter += cpuCycles[CmpZpg];
		CmO(registerA, tempS);
		
	}else if (opeCode == CmpZpgX){
		ZpgO(registerX, &tempS);
		counter += cpuCycles[CmpZpgX];
		CmO(registerA, tempS);
		
	}else if (opeCode == CmpAbs){
		Abs(&tempS);
		counter += cpuCycles[CmpAbs];
		CmO(registerA, tempS);
		
	}else if (opeCode == CmpAbsX){
		counter += AbsO(registerX, &tempS);
		counter += cpuCycles[CmpAbsX];
		CmO(registerA, tempS);

	}else if (opeCode == CmpAbsY){
		counter += AbsO(registerY, &tempS);
		counter += cpuCycles[CmpAbsY];
		CmO(registerA, tempS);
		
	}else if (opeCode == CmpXInd){
		OInd(registerX, &tempS);
		counter += cpuCycles[CmpXInd];
		CmO(registerA, tempS);
	
	}else if (opeCode == CmpIndY){
		counter += IndO(registerY, &tempS);
		counter += cpuCycles[CmpIndY];
		CmO(registerA, tempS);

	}else if (opeCode == CpXIm){
		counter += cpuCycles[CpXIm];
		CmO(registerX, registerPC);
		registerPC += 1;
		
	}else if (opeCode == CpXZpg){
		Zpg(&tempS);
		counter += cpuCycles[CpXZpg];
		CmO(registerX, tempS);
		
	}else if (opeCode == CpXAbs){
		Abs(&tempS);
		counter += cpuCycles[CpXAbs];
		CmO(registerX, tempS);
		
	}else if (opeCode == CpYIm){
		counter += cpuCycles[CpYIm];
		CmO(registerY, registerPC);
		registerPC += 1;
		
	}else if (opeCode == CpYZpg){
		Zpg(&tempS);
		counter += cpuCycles[CpYZpg];
		CmO(registerY, tempS);
		
	}else if (opeCode == CpYAbs){
		Abs(&tempS);
		counter += cpuCycles[CpYAbs];
		CmO(registerY, tempS);
		
	}else if (opeCode == PhaImp){
		PhO(registerA);
		counter += cpuCycles[PhaImp];
		
	}else if (opeCode == PhpImp){
		StatusRegi(bFlg, true);
		PhO(registerP);
		counter += cpuCycles[PhpImp];
		
	}else if (opeCode == PlaImp){
		PlO(&registerA);
		counter += cpuCycles[PlaImp];
		
	}else if (opeCode == PlpImp){
		PlO(&registerP);
		counter += cpuCycles[PlpImp];
		
	}else if (opeCode == JmpAbs){
		Abs(&tempS);
		counter += cpuCycles[JmpAbs];
		Jmp(tempS);
		
	}else if (opeCode == JmpInd){
		Ind(&tempS);
		counter += cpuCycles[JmpInd];
		Jmp(tempS);
		
	}else if(opeCode == JsrAbs){
		Abs(&tempS);
		counter += cpuCycles[JsrAbs];
		Jsr(tempS);
	
	}else if (opeCode == RtsImp){
		counter += cpuCycles[RtsImp];
		Rts();
		
	}else if (opeCode == RtiImp){
		counter += cpuCycles[RtiImp];
		Rti();
	
	}else if (opeCode == BccRel){
		counter += cpuCycles[BccRel];
		counter += Bxx(cFlg, false);
		
	}else if (opeCode == BcsRel){
		counter += cpuCycles[BcsRel];
		counter += Bxx(cFlg, true);
	
	}else if (opeCode == BeqRel){
		counter += cpuCycles[BeqRel];
		counter += Bxx(zFlg, true);
		
	}else if (opeCode == BneRel){
		counter += cpuCycles[BneRel];
		counter += Bxx(zFlg, false);
		
	}else if (opeCode == BvcRel){
		counter += cpuCycles[BvcRel];
		counter += Bxx(vFlg, false);
		
	}else if (opeCode == BvsRel){
		counter += cpuCycles[BvsRel];
		counter += Bxx(vFlg, true);
	
	}else if (opeCode == BplRel){
		counter += cpuCycles[BplRel];
		counter += Bxx(nFlg, false);

	}else if (opeCode == BmiRel){
		counter += cpuCycles[BmiRel];
		counter += Bxx(nFlg, true);
		
	}else if (opeCode == SecImp){
		StatusRegi(cFlg, true);
		counter += cpuCycles[SecImp];
		
	}else if (opeCode == ClcImp){
		StatusRegi(cFlg, false);
		counter += cpuCycles[ClcImp];
		
	}else if (opeCode == SeiImp){
		StatusRegi(iFlg, true);
		counter += cpuCycles[SeiImp];
		
	}else if (opeCode == CliImp){
		StatusRegi(iFlg, false);
		counter += cpuCycles[CliImp];
		
	}else if (opeCode == SedImp){
		StatusRegi(dFlg, true);
		counter += cpuCycles[SedImp];
		
	}else if (opeCode == CldImp){
		StatusRegi(dFlg, false);
		counter += cpuCycles[CldImp];
		
	}else if (opeCode == ClvImp){
		StatusRegi(vFlg, false);
		counter += cpuCycles[ClvImp];

	}else if (opeCode == BrkImp){
		if (iFlg == false){
			StatusRegi(bFlg, true);
			Brk(0xfffe);
			counter += cpuCycles[BrkImp];
		}
		
	}else if (opeCode == NopImp){
			counter += cpuCycles[NopImp];

	}else{
		std::cout << "error_" << std::hex << +opeCode << "PG_" << registerPC << std::endl;
	}
	StatusRegi(rFlg, true);
	Interrupt();
	return counter;
}
