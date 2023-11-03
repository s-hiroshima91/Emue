#include <iostream>
#include "cpu.hpp"
#include "ioport.hpp"
#include "mapper.hpp"
#include "common.hpp"

//以下、cpuの操作

/*対象のレジスタの値をレジスタに入力*/
void Cpu::TrO(uint8_t *wRegister, uint8_t operand){
	bool status;
	*wRegister = operand;
	status = CheckZero(*wRegister);
	StatusRegi(zFlg, status);
	status = CheckBit(*wRegister, 7);
	StatusRegi(nFlg, status);
}

/*対象の値をレジスタに入力*/
void Cpu::LdO(uint8_t *wRegister, uint16_t addr){
	uint8_t value;
	value = *MemoryMap(addr);
	TrO(wRegister, value);
}

/*レジスタの値を対象に入力*/
int Cpu::StO(uint16_t wAddr, uint8_t rRegister){
	uint8_t *ptr;
	if (wAddr < 0x2000){
		ptr = MemoryMap(wAddr);
		*ptr = rRegister;
	}else if (wAddr < 0x4020){
		ioPort->IOWrite(rRegister, wAddr);
		if (wAddr == 0x4014){
			return DMA();
		}
		
	}else if (wAddr < 0x8000){		
		ptr = MemoryMap(wAddr);
		*ptr = rRegister;
	}else{
		mapper->MapperCall(rRegister, wAddr);
	}
	return 0;
}

/*対象の値をレジスタAに加算する*/
void Cpu::AdcBase(uint8_t *wRegister, uint8_t operand){
	uint16_t calc;
	bool flgA, flgB, cStatu, zStatu ,vStatu, nStatu;
	/*キャリーオーバを防ぐためshortで計算する*/
	calc = static_cast<uint16_t>(*wRegister) + static_cast<uint16_t>(operand);
	cStatu = CheckBit(registerP, cFlg);
	calc += static_cast<uint16_t>(cStatu);
	cStatu = static_cast<bool>(calc >> 8);
	flgA = CheckBit(*wRegister, 7);
	flgB = CheckBit(operand, 7);
	*wRegister = static_cast<uint8_t>(calc & 0x00ff);
	nStatu = CheckBit(*wRegister, 7);
	vStatu = flgA & flgB & (! nStatu);
	vStatu |= ((! flgA) & (! flgB) & nStatu);
	zStatu = CheckZero(*wRegister);
	StatusRegi(cFlg, cStatu);
	StatusRegi(zFlg, zStatu);
	StatusRegi(vFlg, vStatu);
	StatusRegi(nFlg, nStatu);
}

void Cpu::Adc(uint8_t *wRegister, uint16_t addr){
	uint8_t value;
	value = *MemoryMap(addr);
	AdcBase(wRegister, value);
}

/*対象の値をレジスタAから引く*/
void Cpu::Sbc(uint8_t *wRegister, uint16_t addr){
	uint8_t value;
	/*わたす値を2の補数にすることで足し算と共通の計算とする。*/
	value = *MemoryMap(addr);
	value ^= 0xff; //2の補数計算
	//operand++;  ここで1を足さないことで足し算とキャリーフラグの扱いを共通化
	AdcBase(wRegister, value);
}

/*対象の値とレジスタAのAnd*/
void Cpu::And(uint16_t addr){
	bool status; 
	registerA &= *MemoryMap(addr);
	status = CheckZero(registerA);
	StatusRegi(zFlg, status);
	status = CheckBit(registerA, 7);
	StatusRegi(nFlg, status);
}

/*対象の値とレジスタAのOr*/
void Cpu::Ora(uint16_t addr){
	bool status;
	registerA |= *MemoryMap(addr);
	status = CheckZero(registerA);
	StatusRegi(zFlg, status);
	status = CheckBit(registerA, 7);
	StatusRegi(nFlg, status);
}

/*対象の値とレジスタAのXor*/
void Cpu::Eor(uint16_t addr){
	bool status;
	registerA ^= *MemoryMap(addr);
	status = CheckZero(registerA);
	StatusRegi(zFlg, status);
	status = CheckBit(registerA, 7);
	StatusRegi(nFlg, status);
}

/*対象の値を左シフト*/
void Cpu::AslA(uint8_t *operand){
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
void Cpu::Asl(uint16_t addr){
	uint8_t *ptr;
	ptr = MemoryMap(addr);
	AslA(ptr);
}

/*対象の値を右シフト*/
void Cpu::LsrA(uint8_t *operand){
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
void Cpu::Lsr(uint16_t addr){
	uint8_t *ptr;
	ptr = MemoryMap(addr);
	LsrA(ptr);
}

/*対象の値をキャリー含めて左シフト*/
void Cpu::RolA(uint8_t *operand){
	bool status;
	uint8_t  temp;
	status = CheckBit(registerP, cFlg);
	temp = static_cast<uint8_t>(status);
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
void Cpu::Rol(uint16_t addr){
	uint8_t *ptr;
	ptr = MemoryMap(addr);
	RolA(ptr);
}

/*対象の値をキャリー含めて右シフト*/
void Cpu::RorA(uint8_t *operand){
	bool status;
	uint8_t temp;
	status = CheckBit(registerP, cFlg);
	temp = static_cast<uint8_t>(status);
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
void Cpu::Ror(uint16_t addr){
	uint8_t *ptr;
	ptr = MemoryMap(addr);
	RorA(ptr);
}

/*対象のbitを確認*/
void Cpu::Bit(uint16_t addr){
	bool status;
	uint8_t value;
	value = *MemoryMap(addr);
	status = CheckBit(value, 7);
	StatusRegi(nFlg, status);
	status = CheckBit(value, 6);
	StatusRegi(vFlg, status);
	value &= registerA;
	status = CheckZero(value);
	StatusRegi(zFlg, status);
}

/*対象の値と対象のレジスタの値を比較*/
void Cpu::CmO(uint8_t rRegister, uint16_t rMemory){
	uint8_t temp;
	bool Vtemp;
	temp = rRegister; //レジスタの値を書き換えない
	Vtemp = CheckBit(registerP, vFlg);
	StatusRegi(cFlg, true); //ボローなし引き算のためキャリーをセット
	Sbc(&temp, rMemory);
	StatusRegi(vFlg, Vtemp);
}

/*対象の値をインクリメント*/
void Cpu::InO(uint8_t *addr){
	bool status;
	*addr += 1;
	status = CheckZero(*addr);
	StatusRegi(zFlg, status);
	status = CheckBit(*addr, 7);
	StatusRegi(nFlg, status);
}

/*対象の値をインクリメント*/
void Cpu::Inc(uint16_t addr){
	uint8_t *ptr;
	ptr = MemoryMap(addr);
	InO(ptr);
}

/*対象の値をデクリメント*/
void Cpu::DeO(uint8_t *addr){
	bool status;
	*addr -= 1;
	status = CheckBit(*addr, 7);
	StatusRegi(nFlg, status);
	status = CheckZero(*addr);
	StatusRegi(zFlg, status);
}

/*対象の値をデクリメント*/
void Cpu::Dec(uint16_t addr){
	uint8_t *ptr;
	ptr = MemoryMap(addr);
	DeO(ptr);
}

/*レジスタの値をスタック*/
void Cpu::PhO(uint8_t rRegister){
	uint16_t addr;
	uint8_t *ptr;
	/*スタックのアドレスに変換*/
	addr = static_cast<uint16_t>(registerS) | 0x0100;
	ptr = MemoryMap(addr);
	*ptr = rRegister;
	registerS -= 1; //スタックポインタをデクリメント
}

/*スタックをレジスタの値に入力*/
void Cpu::PlO(uint8_t *wRegister){
	uint16_t addr;
	bool status;
	uint8_t value;
	//スタックポインタをインクリメント
	registerS += 1;
	addr = static_cast<uint16_t>(registerS) | 0x0100;
	value = *MemoryMap(addr);
	status = CheckBit(value, 7);
	StatusRegi(nFlg,status);
	status = CheckZero(value);
	StatusRegi(zFlg, status);
	*wRegister = value;
}

/*ジャンプ*/
void Cpu::Jmp(uint16_t addr){
	registerPC = addr;
}

/*サブルーチン呼び出し*/
void Cpu::Jsr(uint16_t addr){
	uint8_t value;
	registerPC -= 1;
	/*プログラムカウンタの値をスタック*/
	value = static_cast<uint8_t>((registerPC >> 8) & 0x00ff);
	PhO(value);
	value = static_cast<uint8_t>(0x00ff & registerPC);
	PhO(value);
	registerPC = addr;
}
/*サブルーチンからの帰還*/
void Cpu::Rts(){
	uint8_t value;
	uint8_t temp;
	temp = registerP;
	/*プログラムカウンタの値を復帰*/
	PlO(&value);
	registerPC = static_cast<uint16_t>(value);
	PlO(&value);
	registerPC |= (static_cast<uint16_t>(value) << 8);
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
	uint8_t value;
	temp = CheckBit(registerP, flg);
	if (temp == status){
		value = *MemoryMap(registerPC);
		registerPC += C2S(value);
		registerPC += 1;
		return 1 * 3;
	}
	registerPC += 1;
	return 0;
}

/*ソフトウェア割り込み*/
void Cpu::Brk(uint16_t addr){
	registerPC += 1;
	Jsr(addr);
	PhO(registerP);
	StatusRegi(iFlg, true);
	registerPC = PrgCounter(MemoryMap(addr));
}

/*スプライトデータ転送*/
int Cpu::DMA(){
	uint16_t addr;
	uint8_t *ptr;
	addr = static_cast<uint16_t>(*ioPort->ioReg[0x24]) << 8;
	ptr = MemoryMap(addr);
	ioPort->DMAFunc(ptr);
	return 513 * 3;
}


//以下、アドレッシング
void Cpu::Zpg(uint16_t *addr){
	uint8_t temp;
	temp = *MemoryMap(registerPC);
	registerPC += 1;
	*addr = static_cast<uint16_t>(temp);
}


void Cpu::ZpgO(uint8_t operand, uint16_t *addr){
	uint16_t temp;
	Zpg(&temp);
	temp += static_cast<uint16_t>(operand);
	*addr = temp & 0x00ff;
}

void Cpu::Abs(uint16_t *addr){
	uint16_t temp;
	Zpg(addr);
	Zpg(&temp);
	*addr |= (temp << 8);
}

int Cpu::AbsO(uint8_t operand, uint16_t *addr){
	uint16_t temp;
	Abs(&temp);
	*addr = temp + static_cast<uint16_t>(operand);
	if ((*addr & 0x0100) == (temp & 0x0100)){
		return 0;
	}
	return 1 * 3;
}
	
void Cpu::OInd(uint8_t operand, uint16_t *addr){
	uint16_t addr1;
	uint8_t addr2;
	Zpg(&addr1);
	addr1 = (addr1 + operand) & 0x00ff;
	addr2 = *MemoryMap((addr1 + 1) & 0x00ff);
	addr1 = static_cast<uint16_t>(*MemoryMap(addr1));
	*addr = addr1 + (static_cast<uint16_t>(addr2) << 8);
}

int Cpu::IndO(uint8_t operand, uint16_t *addr){
	uint16_t addr1;
	uint8_t addr2;
	Zpg(&addr1);
	addr2 = *MemoryMap((addr1 + 1) & 0x00ff);
	addr1 = static_cast<uint16_t>(*MemoryMap(addr1));
	addr1 += (static_cast<uint16_t>(addr2) << 8);
	*addr = addr1 + static_cast<uint16_t>(operand);
	if ((*addr & 0x0100) == (addr1 & 0x0100)){
		return 0;
	}
	return 1 * 3;
}

void Cpu::Ind(uint16_t *addr){
	uint16_t addr1;
	uint8_t addr2;
	Abs(&addr1);
	if ((addr1 & 0xff) == 0xff){
		addr2 = *MemoryMap(addr1- 0xff);
	}else{
		addr2 = *MemoryMap(addr1+ 1);
	}
	addr1 = static_cast<uint16_t>(*MemoryMap(addr1));
	*addr = addr1 + (static_cast<uint16_t>(addr2) << 8);
}

