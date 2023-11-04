#include "cpu.hpp"
#include "cpuinline.hpp"
#include "cartridge.hpp"
#include <iostream>


char Cpu::Imm(){
	char value;
	value = *MemoryMap(registerPC);
	++registerPC;
	return value;
}

ushort Cpu::Abs(){
	char value;
	value = Imm();
	return Byte1to2(Imm(), value);
}

void Cpu::Adc(char value){
	bool carry;
	carry = CheckBit(registerF, cFlg);	
	ushort result;
	registerF &= 0x0f;
	if (carry){
		registerF |= HalfCarry2(registerA, value);
		result = C2US(value) + 1;
	}else{
		registerF |= HalfCarry(registerA, value);
		result = C2US(value);
	}
	result += C2US(registerA);
	
	if ((result & 0x0100) == 0x0100){
		registerF |= 0x10;
	}
	registerA = static_cast<char>(result);
	
	if (registerA == 0){
		registerF |= 0x80; //0b1000 0000
	}
}

void Cpu::Add(char value){
	ushort result;
	registerF &= 0x0f;
	registerF |= HalfCarry(registerA, value);
	result = C2US(registerA) + C2US(value);
	if ((result & 0x0100) == 0x0100){
		registerF |= 0x10;
	}
	registerA = static_cast<char>(result);
	
	if (registerA == 0){
		registerF |= 0x80; //0b1000 0000
	}
}

void Cpu::Sbc(char value){
	value ^= 0xff;
	registerF ^= 0x10;
	Adc(value);
	registerF ^= 0x70;
}
	
void Cpu::Sub(char value){
	value ^= 0xff;
	registerF |= 0x10;
	Adc(value);
	registerF ^= 0x70;
}

void Cpu::Add16(ushort *registerXY, ushort registerZW){
	int result, value1, value2;
	
	registerF &= 0x80; //0b1000 0000
	value1 = static_cast<int>(registerZW);
	value2 = static_cast<int>(*registerXY);
	
	result = value1 + value2;
	*registerXY = static_cast<ushort>(result);
	
	value1 = (value1 & 0x00000fff) + (value2 & 0x00000fff);
	value1 = (value1 & 0x00001000) >> 7;
	value1 += (result & 0x00010000) >> 12;
	
	registerF |= static_cast<char>(value1);
}

void Cpu::Inc(char *value){
	registerF &= 0x1f;// 0b0001 1111
	registerF |= HalfCarry(*value, 1);
	*value += 1;
	if (*value == 0){
		registerF |= 0x80; //0b1000 0000
	}
}

void Cpu::Dec(char *value){
	registerF |= 0xe0; //0b1110 0000
	registerF ^= HalfCarry(*value, 0xff);
	*value -= 1;
	if (*value != 0){
		registerF &= 0x7f; //0b0111 1111
	}
}

void Cpu::And(char value){
	registerA &= value;
	registerF &= 0x0f;
	if (registerA == 0){
		registerF |= 0xa0; //0b1010 0000
	}else{
		registerF |= 0x20;
	}
}

void Cpu::Or(char value){
	registerA |= value;
	registerF &= 0x0f;
	if (registerA == 0){
		registerF |= 0x80;
	}
}

void Cpu::Xor(char value){
	registerA ^= value;
	registerF &= 0x0f;
	if (registerA == 0){
		registerF |= 0x80;
	}
}

void Cpu::Cp(char value){
	char temp;
	temp = registerA;
	Sub(value);
	registerA = temp;
}

void Cpu::Rlc(char *value, char carry){
	registerF &= 0x0f;
	registerF |= ((*value & 0x80) >> 3);
	*value <<= 1;
	*value |= (carry >> 4);
}

void Cpu::RlcX(char *value){
	char carry;
	carry = ((*value & 0x80) >> 3);
	Rlc(value, carry);
}

void Cpu::RlX(char *value){
	char carry;
	carry = 0x10 & registerF;
	Rlc(value, carry);
}

void Cpu::Rrc(char *value, char carry){
	registerF &= 0x0f;
	registerF |= ((*value & 0x01) << 4);
	*value = (*value >> 1) & 0x7f;
	*value |= (carry << 3);
}

void Cpu::RrcX(char *value){
	char carry;
	carry = ((*value & 0x01) << 4);
	Rrc(value, carry);
}

void Cpu::RrX(char *value){
	char carry;
	carry = 0x10 & registerF;
	Rrc(value, carry);
}

void Cpu::SraX(char *value){
	char carry;
	carry =  (*value & 0x80) >> 3;
	Rrc(value, carry);
}

void Cpu::SlaX(char *value){
	Rlc(value, 0);
}

void Cpu::SrlX(char *value){
	Rrc(value, 0);
}

void Cpu::LdR88Imm(char *registerX, char *registerY){
	*registerY = Imm();
	*registerX = Imm();
}

void Cpu::LdR16Imm(ushort *registerXY){
	char upper, lower;
	lower = Imm();
	upper = Imm();
	*registerXY = Byte1to2(upper, lower);
}
	

void Cpu ::StX16(ushort addr, ushort value){
	*MemoryMap(addr) = static_cast<char>(0x00ff & value);
	*MemoryMap(addr + 1) = static_cast<char>(0x00ff & (value >> 8));	
}

int Cpu::StX(ushort addr, char value){
	if (addr < 0x8000){
		Crg->MBC(addr, value);
		return 0;
		
	}else if ((addr >= 0xa000) && (addr < 0xc000)){
		if (Crg->ramStat == 0){
			*MemoryMap(addr) = value;
		}else if (Crg->ramStat < 0xff00){
			Crg->UpdateReg();
			sRam[0] = value;//Crg->RTCWrite(value);
		}
		return 0;
		
	}else if (addr < 0xff00){
		*MemoryMap(addr) = value;
		return 0;
		
	}else{
		return IOReg(addr, value);
	}
}

void Cpu::Pop88(char *registerX, char *registerY){
	*registerY = *MemoryMap(registerSP);
	++registerSP;
	*registerX = *MemoryMap(registerSP);
	++registerSP;
}

void Cpu::Push88(char registerX, char registerY){
	--registerSP;
	*MemoryMap(registerSP) = registerX;
	--registerSP;
	*MemoryMap(registerSP) = registerY;
}
	
void Cpu::Push16(ushort registerXY){
	char upper, lower;
	upper = static_cast<char>(registerXY >> 8);
	lower = static_cast<char>(registerXY);
	Push88(upper, lower);
}

int Cpu::JpBranch(int flg, bool status){
	bool regFlg;
	regFlg = CheckBit(registerF, flg);
	
	if(regFlg == status){
		Jp();
		return 4;
	}else{
		registerPC += 2;
		return 0;
	}
}

int Cpu::JrBranch(int flg, bool status){
	bool regFlg;
	regFlg = CheckBit(registerF, flg);
	
	if(regFlg == status){
		Jr();
		return 4;
	}else{
		++registerPC;
		return 0;
	}
}

int Cpu::CallBranch(int flg, bool status){
	bool regFlg;
	regFlg = CheckBit(registerF, flg);
	
	if(regFlg == status){
		Call();
		return 12;
	}else{
		registerPC += 2;
		return 0;
	}
}

int Cpu::RetBranch(int flg, bool status){
	bool regFlg;
	regFlg = CheckBit(registerF, flg);
	
	if(regFlg == status){
		Ret();
		return 12;
	}else{
		return 0;
	}
}

void Cpu::Swap(char *value){
	registerF &= 0x0f;
	*value = (*value << 4) | ((*value >> 4) & 0x0f);
}
	