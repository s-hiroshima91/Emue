#include "cpu.hpp"
#include "cpuinline.hpp"
#include <iostream>

int Cpu::OpError(){
	std::cout << "error " << std::hex << registerPC << std::endl;
	return 0;
}

/*0x00*/
int Cpu::Nop(){
	return 0;
}

int Cpu::LdBCImm(){
	LdR88Imm(&registerB, &registerC);
	return 0;
}

int Cpu::LdbcA(){
	ushort addr;
	addr = Byte1to2(registerB, registerC);
	return StX(addr, registerA);
}

int Cpu::IncBC(){
	Inc16(&registerB, &registerC);
	return 0;
}

int Cpu::IncB(){
	Inc(&registerB);
	return 0;
}

int Cpu::DecB(){
	Dec(&registerB);
	return 0;
}

int Cpu::LdBImm(){
	registerB = Imm();
	return 0;
}

int Cpu::RlcA(){
	RlcX(&registerA);
	return 0;
}

int Cpu::LdAbsSP(){
	ushort addr;
	addr = Abs();
	StX16(addr, registerSP);
	return 0;
}

int Cpu::AddHLBC(){
	Add16(&registerHL, Byte1to2(registerB, registerC));
	return 0;
}

int Cpu::LdAbc(){
	registerA = *MemoryMap(Byte1to2(registerB, registerC));
	return 0;
}

int Cpu::DecBC(){
	Dec16(&registerB, &registerC);
	return 0;
}

int Cpu::IncC(){
	Inc(&registerC);
	return 0;
}

int Cpu::DecC(){
	Dec(&registerC);
	return 0;
}

int Cpu::LdCImm(){
	registerC = Imm();
	return 0;
}

int Cpu::RrcA(){
	RrcX(&registerA);
	return 0;
}

/*0x10*/
int Cpu::StopCpu(){
	clockCounter = 0;
	std::cout << "stop" << std::hex << registerPC << std::endl;
//	++registerPC;
	if (!CheckBit(ioReg[0x4d], 0)){
		return 4;
	}
	
	if (CheckBit(ioReg[0x4d], 7)){
		doubleSpeed = 0;
		ioReg[0x4d] = 0x00;
		return 0;//16200;
		
	}else{
		doubleSpeed = 1;
		ioReg[0x4d] = 0x80;
		return 0;//8400;
		
	}
}

int Cpu::LdDEImm(){
	LdR88Imm(&registerD, &registerE);
	return 0;
}

int Cpu::LddeA(){
	ushort addr;
	addr = Byte1to2(registerD, registerE);
	return StX(addr, registerA);
}

int Cpu::IncDE(){
	Inc16(&registerD, &registerE);
	return 0;
}

int Cpu::IncD(){
	Inc(&registerD);
	return 0;
}

int Cpu::DecD(){
	Dec(&registerD);
	return 0;
}

int Cpu::LdDImm(){
	registerD = Imm();
	return 0;
}

int Cpu::RlA(){
	RlX(&registerA);
	return 0;
}

int Cpu::Jr(){
	ushort value;
	value = Imm();
	registerPC += C2S(value);
	return 0;
}

int Cpu::AddHLDE(){
	Add16(&registerHL, Byte1to2(registerD, registerE));
	return 0;
}

int Cpu::LdAde(){
	registerA = *MemoryMap(Byte1to2(registerD, registerE));
	return 0;
}

int Cpu::DecDE(){
	Dec16(&registerD, &registerE);
	return 0;
}

int Cpu::IncE(){
	Inc(&registerE);
	return 0;
}

int Cpu::DecE(){
	Dec(&registerE);
	return 0;
}

int Cpu::LdEImm(){
	registerE = Imm();
	return 0;
}

int Cpu::RrA(){
	RrX(&registerA);
	return 0;
}

/*0x20*/
int Cpu::JrNZ(){
	return JrBranch(zFlg, false);
}

int Cpu::LdHLImm(){
	LdR16Imm(&registerHL);
	return 0;
}

int Cpu::LdhliA(){
	int counter;
	counter = StX(registerHL, registerA);
	++registerHL;
	return counter;
}

int Cpu::IncHL(){
	++registerHL;
	return 0;
}

int Cpu::IncH(){
	char value;
	value = static_cast<char>(registerHL >> 8);
	Inc(&value);
	registerHL &= 0x00ff;
	registerHL |= static_cast<ushort>(value) << 8;
	return 0;
}

int Cpu::DecH(){
	char value;
	value = static_cast<char>(registerHL >> 8);
	Dec(&value);
	registerHL &= 0x00ff;
	registerHL |= static_cast<ushort>(value) << 8;
	return 0;
}

int Cpu::LdHImm(){
	registerHL &= 0x00ff;
	registerHL |= static_cast<ushort>(Imm()) << 8;
	return 0;
}

int Cpu::Daa(){
	char upper, lower;
	int flg;
	
	flg = static_cast<int>((registerF) >> 4) & 0x03;
	
	upper = (registerA >> 4) & 0x0f;
	lower = registerA & 0x0f;
	if (CheckBit(registerF, nFlg)){
		registerA += daaN[flg];
		
	}else{
		if (lower < 0x0a){
			if (upper < 0x0a){
			
			}else{
				flg += 8;
				registerF |= 0x10;
			}
		}else{
			if (upper < 0x09){
				flg += 4;
			}else{
				flg += 12;
				registerF |= 0x10;
			}
		}
		registerA += daaP[flg];
	}
	
	registerF &= 0x50; //0b0101 0000
	if (registerA == 0){
		registerF |= 0x80;
	}

	return 0;
}

int Cpu::JrZ(){
	return JrBranch(zFlg, true);
}

int Cpu::AddHLHL(){
	Add16(&registerHL, registerHL);
	return 0;
}

int Cpu::LdAhli(){
	registerA = *MemoryMap(registerHL);
	++registerHL;
	return 0;
}

int Cpu::DecHL(){
	--registerHL;
	return 0;
}

int Cpu::IncL(){
	char value;
	value = static_cast<char>(registerHL);
	Inc(&value);
	registerHL &= 0xff00;
	registerHL |= C2US(value);
	return 0;
}

int Cpu::DecL(){
	char value;
	value = static_cast<char>(registerHL);
	Dec(&value);
	registerHL &= 0xff00;
	registerHL |= C2US(value);
	return 0;
}

int Cpu::LdLImm(){
	registerHL &= 0xff00;
	registerHL |= C2US(Imm());
	return 0;
}

int Cpu::Cpl(){
	registerA ^= 0xff;
	registerF |= 0x60; //0b0110 0000
	return 0;
}

/*0x30*/

int Cpu::JrNC(){
	return JrBranch(cFlg, false);
}

int Cpu::LdSPImm(){
	LdR16Imm(&registerSP);
	return 0;
}

int Cpu::LdhldA(){
	int counter;
	counter = StX(registerHL, registerA);
	--registerHL;
	return counter;
}

int Cpu::IncSP(){
	++registerSP;
	return 0;
}

int Cpu::Inchl(){
	char *ptr;
	ptr = MemoryMap(registerHL);
	Inc(ptr);
	return 0;
}

int Cpu::Dechl(){
	char *ptr;
	ptr = MemoryMap(registerHL);
	Dec(ptr);
	return 0;
}

int Cpu::LdhlImm(){
	char value;
	value = Imm();
	return StX(registerHL, value);
}

int Cpu::Scf(){
	registerF &= 0x8f; // 0b1000 1111
	registerF |= 0x10;
	return 0;
}

int Cpu::JrC(){
	return JrBranch(cFlg, true);
}

int Cpu::AddHLSP(){
	Add16(&registerHL, registerSP);
	return 0;
}

int Cpu::LdAhld(){
	registerA = *MemoryMap(registerHL);
	--registerHL;
	return 0;
}

int Cpu::DecSP(){
	--registerSP;
	return 0;
}

int Cpu::IncA(){
	Inc(&registerA);
	return 0;
}

int Cpu::DecA(){
	Dec(&registerA);
	return 0;
}

int Cpu::LdAImm(){
	registerA = Imm();
	return 0;
}

int Cpu::Ccf(){
	registerF &= 0x9f; //0b1001 0000
	registerF ^= 0x10; //0b0001 0000
	return 0;
}

/*0x40*/
/* int Cpu::LdBB(){
	return 0;
	}
	equiv Nop
*/

int Cpu::LdBC(){
	registerB = registerC;
	return 0;
}

int Cpu::LdBD(){
	registerB = registerD;
	return 0;
}

int Cpu::LdBE(){
	registerB = registerE;
	return 0;
}

int Cpu::LdBH(){
	registerB = static_cast<char>(registerHL >> 8);
	return 0;
}

int Cpu::LdBL(){
	registerB = static_cast<char>(registerHL);
	return 0;
}

int Cpu::LdBhl(){
	registerB = *MemoryMap(registerHL);
	return 0;
}

int Cpu::LdBA(){
	registerB = registerA;
	return 0;
}

int Cpu::LdCB(){
	registerC = registerB;
	return 0;
}

/*int Cpu::LdCC(){
	registerC = registerC;
	return 0;
}
equiv Nop
*/

int Cpu::LdCD(){
	registerC = registerD;
	return 0;
}

int Cpu::LdCE(){
	registerC = registerE;
	return 0;
}

int Cpu::LdCH(){
	registerC = static_cast<char>(registerHL >> 8);
	return 0;
}

int Cpu::LdCL(){
	registerC = static_cast<char>(registerHL);
	return 0;
}

int Cpu::LdChl(){
	registerC = *MemoryMap(registerHL);
	return 0;
}

int Cpu::LdCA(){
	registerC = registerA;
	return 0;
}

/*0x50*/
int Cpu::LdDB(){
	registerD = registerB;
	return 0;
}

int Cpu::LdDC(){
	registerD = registerC;
	return 0;
}

/*int Cpu::LdDD(){
	registerD = registerD;
	return 0;
}
equiv Nop
*/

int Cpu::LdDE(){
	registerD = registerE;
	return 0;
}

int Cpu::LdDH(){
	registerD = static_cast<char>(registerHL >> 8);
	return 0;
}

int Cpu::LdDL(){
	registerD = static_cast<char>(registerHL);
	return 0;
}

int Cpu::LdDhl(){
	registerD = *MemoryMap(registerHL);
	return 0;
}

int Cpu::LdDA(){
	registerD = registerA;
	return 0;
}

int Cpu::LdEB(){
	registerE = registerB;
	return 0;
}

int Cpu::LdEC(){
	registerE = registerC;
	return 0;
}

int Cpu::LdED(){
	registerE = registerD;
	return 0;
}

/*int Cpu::LdEE(){
	registerC = registerE;
	return 0;
}
equiv Nop
*/

int Cpu::LdEH(){
	registerE = static_cast<char>(registerHL >> 8);
	return 0;
}

int Cpu::LdEL(){
	registerE = static_cast<char>(registerHL);
	return 0;
}

int Cpu::LdEhl(){
	registerE = *MemoryMap(registerHL);
	return 0;
}

int Cpu::LdEA(){
	registerE = registerA;
	return 0;
}

/*0x60*/
int Cpu::LdHB(){
	LdUpperR8(&registerHL, registerB);
	return 0;
}

int Cpu::LdHC(){
	LdUpperR8(&registerHL, registerC);
	return 0;
}

int Cpu::LdHD(){
	LdUpperR8(&registerHL, registerD);
	return 0;
}

int Cpu::LdHE(){
	LdUpperR8(&registerHL, registerE);
	return 0;
}

/*
int Cpu::LdHH(){
	registerHL = registerHL;
	return 0;
}
equiv Nop*/

int Cpu::LdHL(){
	registerHL &= 0x00ff;
	registerHL |= (registerHL << 8);
	return 0;
}

int Cpu::LdHhl(){
	LdUpperR8(&registerHL, *MemoryMap(registerHL));
	return 0;
}

int Cpu::LdHA(){
	LdUpperR8(&registerHL, registerA);
	return 0;
}

int Cpu::LdLB(){
	LdLowerR8(&registerHL, registerB);
	return 0;
}

int Cpu::LdLC(){
	LdLowerR8(&registerHL, registerC);
	return 0;
}

int Cpu::LdLD(){
	LdLowerR8(&registerHL, registerD);
	return 0;
}

int Cpu::LdLE(){
	LdLowerR8(&registerHL, registerE);
	return 0;
}

int Cpu::LdLH(){
	registerHL &= 0xff00;
	registerHL |= 0x00ff & (registerHL >> 8);
	return 0;
}

/*
int Cpu::LdLL(){
	registerHL = registerHL;
	return 0;
}
equiv Nop
*/

int Cpu::LdLhl(){
	LdLowerR8(&registerHL, *MemoryMap(registerHL));
	return 0;
}

int Cpu::LdLA(){
	LdLowerR8(&registerHL, registerA);
	return 0;
}

/*0x70*/
int Cpu::LdhlB(){
	return StX(registerHL, registerB);
}

int Cpu::LdhlC(){
	return StX(registerHL, registerC);
}

int Cpu::LdhlD(){
	return StX(registerHL, registerD);
}

int Cpu::LdhlE(){
	return StX(registerHL, registerE);
}

int Cpu::LdhlH(){
	return StX(registerHL, static_cast<char>(registerHL >> 8));
}

int Cpu::LdhlL(){
	return StX(registerHL, static_cast<char>(registerHL));
}

int Cpu::Halt(){
	int counter = 0;
	if (ime){
		--registerPC;
		halt = true;
		
	}else{
		if (((IE & IF) & 0x1f) != 0){
			ushort opeCode;
			if (!halt){
				opeCode = C2US(Imm());
				--registerPC;
				counter = (this->*cpuInstrs[opeCode])();
				counter += cpuCycle[opeCode];
			}
			halt = false;

		}else{
			--registerPC;
			halt = true;

		}
	}
	return counter;
}

int Cpu::LdhlA(){
	return StX(registerHL, registerA);
}

int Cpu::LdAB(){
	registerA = registerB;
	return 0;
}

int Cpu::LdAC(){
	registerA = registerC;
	return 0;
}

int Cpu::LdAD(){
	registerA = registerD;
	return 0;
}

int Cpu::LdAE(){
	registerA = registerE;
	return 0;
}

int Cpu::LdAH(){
	registerA = static_cast<char>(registerHL >> 8);
	return 0;
}

int Cpu::LdAL(){
	registerA = static_cast<char>(registerHL);
	return 0;
}

int Cpu::LdAhl(){
	registerA = *MemoryMap(registerHL);
	return 0;
}

/*
int Cpu::LdAA(){
	registerA = registerA;
	return 0;
}
equiv Nop
*/

/*0x80*/
int Cpu::AddB(){
	Add(registerB);
	return 0;
}

int Cpu::AddC(){
	Add(registerC);
	return 0;
}

int Cpu::AddD(){
	Add(registerD);
	return 0;
}

int Cpu::AddE(){
	Add(registerE);
	return 0;
}

int Cpu::AddH(){
	char value;
	value = static_cast<char>(registerHL >> 8);
	Add(value);
	return 0;
}

int Cpu::AddL(){
	char value;
	value = static_cast<char>(registerHL);
	Add(value);
	return 0;
}

int Cpu::Addhl(){
	char value;
	value = *MemoryMap(registerHL);
	Add(value);
	return 0;
}

int Cpu::AddA(){
	Add(registerA);
	return 0;
}

int Cpu::AdcB(){
	Adc(registerB);
	return 0;
}

int Cpu::AdcC(){
	Adc(registerC);
	return 0;
}

int Cpu::AdcD(){
	Adc(registerD);
	return 0;
}

int Cpu::AdcE(){
	Adc(registerE);
	return 0;
}

int Cpu::AdcH(){
	char value;
	value = static_cast<char>(registerHL >> 8);
	Adc(value);
	return 0;
}

int Cpu::AdcL(){
	char value;
	value = static_cast<char>(registerHL);
	Adc(value);
	return 0;
}

int Cpu::Adchl(){
	char value;
	value = *MemoryMap(registerHL);
	Adc(value);
	return 0;
}

int Cpu::AdcA(){
	Adc(registerA);
	return 0;
}

/*0x90*/
int Cpu::SubB(){
	Sub(registerB);
	return 0;
}

int Cpu::SubC(){
	Sub(registerC);
	return 0;
}

int Cpu::SubD(){
	Sub(registerD);
	return 0;
}

int Cpu::SubE(){
	Sub(registerE);
	return 0;
}

int Cpu::SubH(){
	char value;
	value = static_cast<char>(registerHL >> 8);
	Sub(value);
	return 0;
}

int Cpu::SubL(){
	char value;
	value = static_cast<char>(registerHL);
	Sub(value);
	return 0;
}

int Cpu::Subhl(){
	char value;
	value = *MemoryMap(registerHL);
	Sub(value);
	return 0;
}

int Cpu::SubA(){
	Sub(registerA);
	return 0;
}

int Cpu::SbcB(){
	Sbc(registerB);
	return 0;
}

int Cpu::SbcC(){
	Sbc(registerC);
	return 0;
}

int Cpu::SbcD(){
	Sbc(registerD);
	return 0;
}

int Cpu::SbcE(){
	Sbc(registerE);
	return 0;
}

int Cpu::SbcH(){
	char value;
	value = static_cast<char>(registerHL >> 8);
	Sbc(value);
	return 0;
}

int Cpu::SbcL(){
	char value;
	value = static_cast<char>(registerHL);
	Sbc(value);
	return 0;
}

int Cpu::Sbchl(){
	char value;
	value = *MemoryMap(registerHL);
	Sbc(value);
	return 0;
}

int Cpu::SbcA(){
	Sbc(registerA);
	return 0;
}

/*0xa0*/
int Cpu::AndB(){
	And(registerB);
	return 0;
}

int Cpu::AndC(){
	And(registerC);
	return 0;
}

int Cpu::AndD(){
	And(registerD);
	return 0;
}

int Cpu::AndE(){
	And(registerE);
	return 0;
}

int Cpu::AndH(){
	char value;
	value = static_cast<char>(registerHL >> 8);
	And(value);
	return 0;
}

int Cpu::AndL(){
	char value;
	value = static_cast<char>(registerHL);
	And(value);
	return 0;
}

int Cpu::Andhl(){
	char value;
	value = *MemoryMap(registerHL);
	And(value);
	return 0;
}

int Cpu::AndA(){
	And(registerA);
	return 0;
}
int Cpu::XorB(){
	Xor(registerB);
	return 0;
}

int Cpu::XorC(){
	Xor(registerC);
	return 0;
}

int Cpu::XorD(){
	Xor(registerD);
	return 0;
}

int Cpu::XorE(){
	Xor(registerE);
	return 0;
}

int Cpu::XorH(){
	char value;
	value = static_cast<char>(registerHL >> 8);
	Xor(value);
	return 0;
}

int Cpu::XorL(){
	char value;
	value = static_cast<char>(registerHL);
	Xor(value);
	return 0;
}

int Cpu::Xorhl(){
	char value;
	value = *MemoryMap(registerHL);
	Xor(value);
	return 0;
}

int Cpu::XorA(){
	Xor(registerA);
	return 0;
}

/*0xb0*/
int Cpu::OrB(){
	Or(registerB);
	return 0;
}

int Cpu::OrC(){
	Or(registerC);
	return 0;
}

int Cpu::OrD(){
	Or(registerD);
	return 0;
}

int Cpu::OrE(){
	Or(registerE);
	return 0;
}

int Cpu::OrH(){
	char value;
	value = static_cast<char>(registerHL >> 8);
	Or(value);
	return 0;
}

int Cpu::OrL(){
	char value;
	value = static_cast<char>(registerHL);
	Or(value);
	return 0;
}

int Cpu::Orhl(){
	char value;
	value = *MemoryMap(registerHL);
	Or(value);
	return 0;
}

int Cpu::OrA(){
	Or(registerA);
	return 0;
}

int Cpu::CpB(){
	Cp(registerB);
	return 0;
}

int Cpu::CpC(){
	Cp(registerC);
	return 0;
}

int Cpu::CpD(){
	Cp(registerD);
	return 0;
}

int Cpu::CpE(){
	Cp(registerE);
	return 0;
}

int Cpu::CpH(){
	char value;
	value = static_cast<char>(registerHL >> 8);
	Cp(value);
	return 0;
}

int Cpu::CpL(){
	char value;
	value = static_cast<char>(registerHL);
	Cp(value);
	return 0;
}

int Cpu::Cphl(){
	char value;
	value = *MemoryMap(registerHL);
	Cp(value);
	return 0;
}

int Cpu::CpA(){
	Cp(registerA);
	return 0;
}

/*0xc0*/
int Cpu::RetNZ(){
	return RetBranch(zFlg, false);
}

int Cpu::PopBC(){
	Pop88(&registerB, &registerC);
	return 0;
}

int Cpu::JpNZ(){
	return JpBranch(zFlg, false);
}

int Cpu::Jp(){
	LdR16Imm(&registerPC);
	return 0;
}

int Cpu::CallNZ(){
	return CallBranch(zFlg, false);
}

int Cpu::PushBC(){
	Push88(registerB, registerC);
	return 0;
}

int Cpu::AddImm(){
	Add(Imm());
	return 0;
}

int Cpu::Rst00(){
	Push16(registerPC);
	registerPC = 0x0000;
	return 0;
}

int Cpu::RetZ(){
	return RetBranch(zFlg, true);
}

int Cpu::Ret(){
	char upper, lower;
	Pop88(&upper, &lower);
	registerPC = Byte1to2(upper, lower);
	return 0;
}

int Cpu::JpZ(){
	return JpBranch(zFlg, true);
}

int Cpu::Prefix(){
	ushort opeCode, lower, middle, upper;
	opeCode = C2US(Imm());
	lower = opeCode & 0x07;
	middle = (opeCode >> 3) & 0x07;
	upper = opeCode >> 6;
	
	if ((lower & 0x04) == 0) {
		(this->*cbCpuInstrs[upper])(middle, operand[lower]);
	}else if (lower == 0x04){
		char value;
		value = static_cast<char>(registerHL >> 8);
		(this->*cbCpuInstrs[upper])(middle, &value);
		registerHL &= 0x00ff;
		registerHL |= static_cast<ushort>(value) << 8;
	}else if (lower == 0x05){
		char value;
		value = static_cast<char>(registerHL);
		(this->*cbCpuInstrs[upper])(middle, &value);
		registerHL &= 0xff00;
		registerHL |= C2US(value);
	}else if (lower == 0x06){
		char *ptr;
		ptr = MemoryMap(registerHL);
		(this->*cbCpuInstrs[upper])(middle, ptr);
	}else{
		(this->*cbCpuInstrs[upper])(middle, &registerA);
	}
	return cbCpuCycle[(upper << 3) | lower];
}

int Cpu::CallZ(){
	return CallBranch(zFlg, true);
}

int Cpu::Call(){
	ushort addr;
	addr = Abs();
	Push16(registerPC);
	registerPC = addr;
	return 0;
}

int Cpu::AdcImm(){
	Adc(Imm());
	return 0;
}

int Cpu::Rst08(){
	Push16(registerPC);
	registerPC = 0x0008;
	return 0;
}

/*0xd0*/
int Cpu::RetNC(){
	return RetBranch(cFlg, false);
}

int Cpu::PopDE(){
	Pop88(&registerD, &registerE);
	return 0;
}

int Cpu::JpNC(){
	return JpBranch(cFlg, false);
}

/*
0xd3
No OpeCode

*/

int Cpu::CallNC(){
	return CallBranch(cFlg, false);
}

int Cpu::PushDE(){
	Push88(registerD, registerE);
	return 0;
}

int Cpu::SubImm(){
	Sub(Imm());
	return 0;
}

int Cpu::Rst10(){
	Push16(registerPC);
	registerPC = 0x0010;
	return 0;
}

int Cpu::RetC(){
	return RetBranch(cFlg, true);
}

int Cpu::RetI(){
	char upper, lower;
	Pop88(&upper, &lower);
	registerPC = Byte1to2(upper, lower);
	Ei();
	return 0;
}

int Cpu::JpC(){
	return JpBranch(cFlg, true);
}

/*0xdb
No OpCode
*/

int Cpu::CallC(){
	return CallBranch(cFlg, true);
}

/*0xdd
No OpCode
*/

int Cpu::SbcImm(){
	Sbc(Imm());
	return 0;
}

int Cpu::Rst18(){
	Push16(registerPC);
	registerPC = 0x0018;
	return 0;
}

/*0xe0*/
int Cpu::LdffImmA(){
	ushort addr;
	addr = static_cast<ushort>(Imm());
	return IOReg(addr, registerA);
}

int Cpu::PopHL(){
	char upper, lower;
	Pop88(&upper, &lower);
	registerHL = Byte1to2(upper, lower);
	return 0;
}

int Cpu::LdffcA(){
	ushort addr;
	addr = static_cast<ushort>(registerC);
	return IOReg(addr, registerA);
}

/*
0xe3, 0xe4
No OpeCode

*/

int Cpu::PushHL(){
	Push16(registerHL);
	return 0;
}

int Cpu::AndImm(){
	And(Imm());
	return 0;
}

int Cpu::Rst20(){
	Push16(registerPC);
	registerPC = 0x0020;
	return 0;
}

int Cpu::AddSPImm(){
	char value;
	ushort temp;
	registerF &= 0x0f;
	value = Imm();
	registerF |= HalfCarry(static_cast<char>(registerSP), value);
	temp = registerSP & 0xff00;
	registerSP += C2S(value);
	if ((registerSP & 0xff00) != temp){
		registerF |= 0x10;
	}
	if (CheckBit(value, 7)){
		registerF ^= 0x10;
	}
	return 0;
}

int Cpu::JpHL(){
	registerPC = registerHL;
	return 0;
}

int Cpu::LdAbsA(){
	return StX(Abs(), registerA);
}

/*0xeb, 0xec, 0xed
No OpCode
*/

int Cpu::XorImm(){
	Xor(Imm());
	return 0;
}

int Cpu::Rst28(){
	Push16(registerPC);
	registerPC = 0x0028;
	return 0;
}

/*0xf0*/
int Cpu::LdAffImm(){
	ushort addr;
	addr = 0xff00 | static_cast<ushort>(Imm());
	registerA = *MemoryMap(addr);
	return 0;
}

int Cpu::PopAF(){
	Pop88(&registerA, &registerF);
	registerF &= 0xf0;
	return 0;
}

int Cpu::LdAffc(){
	ushort addr;
	addr = 0xff00 | static_cast<ushort>(registerC);
	registerA = *MemoryMap(addr);
	return 0;
}

int Cpu::Di(){
	ime = false;
	return 0;
}
/*
0xf4
No OpeCode

*/

int Cpu::PushAF(){
	Push88(registerA, registerF);
	return 0;
}

int Cpu::OrImm()
{
	Or(Imm());
	return 0;
}

int Cpu::Rst30()
{
	Push16(registerPC);
	registerPC = 0x0030;
	return 0;
}

int Cpu::LdHLSPImm(){
	ushort temp;
	temp = registerSP;
	AddSPImm();
	registerHL = registerSP;
	registerSP = temp;
	return 0;
}

int Cpu::LdSPHL(){
	registerSP = registerHL;
	return 0;
}

int Cpu::LdAAbs(){
	registerA = *MemoryMap(Abs());
	return 0;
}

int Cpu::Ei(){
	ime = true;
	return 0;
}

/* 0xfd
No OpCode
*/

int Cpu::CpImm(){
	Cp(Imm());
	return 0;
}

int Cpu::Rst38(){
	Push16(registerPC);
	registerPC = 0x0038;
	return 0;
}

//prefix
//0x00~0x3f
void Cpu::Shift(ushort num, char *value){
	(this->*shiftFunc[num])(value);
	if (*value == 0){
		registerF |= 0x80;
	}
}

void Cpu::BitCheck(ushort num, char *value){
	registerF &= 0x3f;
	if (CheckBit(*value, static_cast<int>(num))){
		registerF |= 0x20; //0b0010 0000
	}else{
		registerF |= 0xa0; //0x1010 0000
	}
}

void Cpu::BitRes(ushort num, char *value){
	 *value &= 0xff ^ (1 << num);
}
		
void Cpu::BitSet(ushort num, char *value){
	*value |= 1 << num;
}