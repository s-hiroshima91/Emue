#include <iostream>
#include "cpu.hpp"
#include "ioport.hpp"
#include "mapper.hpp"
#include "common.hpp"

//以下、cpuの命令

int Cpu::LdaIm(){ 
	LdO(&registerA, registerPC++);
	return 0;
}
	
int Cpu::LdaZpg(){
	uint16_t temp;
	Zpg(&temp);
	LdO(&registerA, temp);
	return 0;
}
		
int Cpu::LdaZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	LdO(&registerA, temp);
	return 0;
}
		
int Cpu::LdaAbs(){
	uint16_t temp;
	Abs(&temp);
	LdO(&registerA, temp);
	return 0;
}
		
int Cpu::LdaAbsX(){
	int counter;
	uint16_t temp;
	counter = AbsO(registerX, &temp);
	LdO(&registerA, temp);
	return counter;
}

int Cpu::LdaAbsY(){
	int counter;
	uint16_t temp;
	counter = AbsO(registerY, &temp);	
	LdO(&registerA, temp);
	return counter;
}
		
int Cpu::LdaXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	LdO(&registerA, temp);
	return 0;
}
	
int Cpu::LdaIndY(){
	int counter;
	uint16_t temp;
	counter = IndO(registerY, &temp);
	LdO(&registerA, temp);
	return counter;
}
		
int Cpu::LdxIm(){
	LdO(&registerX, registerPC++);
	return 0;
}
		
int Cpu::LdxZpg(){
	uint16_t temp;
	Zpg(&temp);
	LdO(&registerX, temp);
	return 0;
}
		
int Cpu::LdxZpgY(){
	uint16_t temp;
	ZpgO(registerY, &temp);
	LdO(&registerX, temp);
	return 0;
}
		
int Cpu::LdxAbs(){
	uint16_t temp;
	Abs(&temp);
	LdO(&registerX, temp);
	return 0;
}
		
int Cpu::LdxAbsY(){
	int counter;
	uint16_t temp;
	counter = AbsO(registerY, &temp);
	LdO(&registerX, temp);
	return counter;
}

int Cpu::LdyIm(){ 
	LdO(&registerY, registerPC++);
	return 0;
}
	
int Cpu::LdyZpg(){
	uint16_t temp;
	Zpg(&temp);
	LdO(&registerY, temp);
	return 0;
}
		
int Cpu::LdyZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	LdO(&registerY, temp);
	return 0;
}	
		
int Cpu::LdyAbs(){
	uint16_t temp;
	Abs(&temp);
	LdO(&registerY, temp);
	return 0;
}
		
int Cpu::LdyAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	LdO(&registerY, temp);
	return counter;
}

int Cpu::StaZpg(){
	uint16_t temp;
	Zpg(&temp);
	return StO(temp, registerA);
}
		
int Cpu::StaZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	return StO(temp, registerA);
}

int Cpu::StaAbs(){
	uint16_t temp;
	Abs(&temp);
	return StO(temp, registerA);
}

int Cpu::StaAbsX(){
	int counter;
	uint16_t temp;
	counter = AbsO(registerX, &temp);
	 return StO(temp, registerA);
}
		
int Cpu::StaAbsY(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerY, &temp);
	return StO(temp, registerA);
}

int Cpu::StaXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	return StO(temp, registerA);
}

int Cpu::StaIndY(){
	uint16_t temp;
	int counter;
	counter = IndO(registerY, &temp);
	return StO(temp, registerA);
}
		
int Cpu::StxZpg(){
	uint16_t temp;
	Zpg(&temp);
	return StO(temp, registerX);
}
		
int Cpu::StxZpgY(){
	uint16_t temp;
	ZpgO(registerY, &temp);
	return StO(temp, registerX);
}

int Cpu::StxAbs(){
	uint16_t temp;
	Abs(&temp);
	return StO(temp, registerX);
}

int Cpu::StyZpg(){
	uint16_t temp;
	Zpg(&temp);
	return StO(temp, registerY);
}
		
int Cpu::StyZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	return StO(temp, registerY);
}

int Cpu::StyAbs(){
	uint16_t temp;
	Abs(&temp);
	return StO(temp, registerY);
}

int Cpu::TaxImp(){
	TrO(&registerX, registerA);
	return 0;
}

int Cpu::TayImp(){
	TrO(&registerY, registerA);
	return 0;
}
		
int Cpu::TsxImp(){
	TrO(&registerX, registerS);
	return 0;
}

int Cpu::TxaImp(){
	TrO(&registerA, registerX);
	return 0;
}
		
int Cpu::TxsImp(){
	registerS = registerX;
	return 0;
}
		
int Cpu::TyaImp(){
	TrO(&registerA, registerY);
	return 0;
}
		
int Cpu::AdcIm(){
	Adc(&registerA, registerPC++);
	return 0;
}

int Cpu::AdcZpg(){
	uint16_t temp;
	Zpg(&temp);
	Adc(&registerA, temp);
	return 0;
}
		
int Cpu::AdcZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Adc(&registerA, temp);
	return 0;
}

int Cpu::AdcAbs(){
	uint16_t temp;
	Abs(&temp);
	Adc(&registerA, temp);
	return 0;
}
		
int Cpu::AdcAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	Adc(&registerA, temp);
	return counter;
}

int Cpu::AdcAbsY(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerY, &temp);
	Adc(&registerA, temp);
	return counter;
}
		
int Cpu::AdcXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	Adc(&registerA, temp);
	return 0;
}
	
int Cpu::AdcIndY(){
	uint16_t temp;
	int counter;
	counter = IndO(registerY, &temp);
	Adc(&registerA, temp);
	return counter;
}

int Cpu::SbcIm(){
	Sbc(&registerA, registerPC++);
	return 0;
}
		
int Cpu::SbcZpg(){
	uint16_t temp;
	Zpg(&temp);
	Sbc(&registerA, temp);
	return 0;
}
		
int Cpu::SbcZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Sbc(&registerA, temp);
	return 0;
}
		
int Cpu::SbcAbs(){
	uint16_t temp;
	Abs(&temp);
	Sbc(&registerA, temp);
	return 0;
}
		
int Cpu::SbcAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	Sbc(&registerA, temp);
	return counter;
}

int Cpu::SbcAbsY(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerY, &temp);
	Sbc(&registerA, temp);
	return counter;
}
		
int Cpu::SbcXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	Sbc(&registerA, temp);
	return 0;
}
	
int Cpu::SbcIndY(){
	uint16_t temp;
	int counter;
	counter = IndO(registerY, &temp);
	Sbc(&registerA, temp);
	return counter;
}
		
int Cpu::IncZpg(){
	uint16_t temp;
	Zpg(&temp);
	Inc(temp);
	return 0;
}
		
int Cpu::IncZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Inc(temp);
	return 0;
}

int Cpu::IncAbs(){
	uint16_t temp;
	Abs(&temp);
	Inc(temp);
	return 0;
}

int Cpu::IncAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	Inc(temp);
	return 0;
}

int Cpu::InXImp(){
	InO(&registerX);
	return 0;
}

int Cpu::InYImp(){
	InO(&registerY);
	return 0;
}

int Cpu::DecZpg(){
	uint16_t temp;
	Zpg(&temp);
	Dec(temp);
	return 0;
}
		
int Cpu::DecZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Dec(temp);
	return 0;
}

int Cpu::DecAbs(){
	uint16_t temp;
	Abs(&temp);
	Dec(temp);
	return 0;
}

int Cpu::DecAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	Dec(temp);
	return 0;
}

int Cpu::DeXImp(){
	DeO(&registerX);
	return 0;
}

int Cpu::DeYImp(){
	DeO(&registerY);
	return 0;
}

int Cpu::AndIm(){
	And(registerPC++);
	return 0;
}
		
int Cpu::AndZpg(){
	uint16_t temp;
	Zpg(&temp);
	And(temp);
	return 0;
}

int Cpu::AndZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	And(temp);
	return 0;
}
		
int Cpu::AndAbs(){
	uint16_t temp;
	Abs(&temp);
	And(temp);
	return 0;
}
		
int Cpu::AndAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	And(temp);
	return counter;
}

int Cpu::AndAbsY(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerY, &temp);
	And(temp);
	return counter;
}
		
int Cpu::AndXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	And(temp);
	return 0;
}
	
int Cpu::AndIndY(){
	uint16_t temp;
	int counter;
	counter = IndO(registerY, &temp);
	And(temp);
	return counter;
}
		
int Cpu::OraIm(){
	Ora(registerPC++);
	return 0;
}
		
int Cpu::OraZpg(){
	uint16_t temp;
	Zpg(&temp);
	Ora(temp);
	return 0;
}
		
int Cpu::OraZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Ora(temp);
	return 0;
}
		
int Cpu::OraAbs(){
	uint16_t temp;
	Abs(&temp);
	Ora(temp);
	return 0;
}
		
int Cpu::OraAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	Ora(temp);
	return counter;
}

int Cpu::OraAbsY(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerY, &temp);
	Ora(temp);
	return counter;
}
		
int Cpu::OraXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	Ora(temp);
	return 0;
}

int Cpu::OraIndY(){
	uint16_t temp;
	int counter;
	counter = IndO(registerY, &temp);
	Ora(temp);
	return counter;
}
		
int Cpu::EorIm(){
	Eor(registerPC++);
	return 0;
}
		
int Cpu::EorZpg(){
	uint16_t temp;
	Zpg(&temp);
	Eor(temp);
	return 0;
}
		
int Cpu::EorZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Eor(temp);
	return 0;
}
		
int Cpu::EorAbs(){
	uint16_t temp;
	Abs(&temp);
	Eor(temp);
	return 0;
}
		
int Cpu::EorAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	Eor(temp);
	return counter;
}

int Cpu::EorAbsY(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerY, &temp);
	Eor(temp);
	return counter;
}
		
int Cpu::EorXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	Eor(temp);
	return 0;
}
	
int Cpu::EorIndY(){
	uint16_t temp;
	int counter;
	counter = IndO(registerY, &temp);
	Eor(temp);
	return 0;
}
		
int Cpu::AslAA(){
	AslA(&registerA);
	return 0;
}
		
int Cpu::AslZpg(){
	uint16_t temp;
	Zpg(&temp);
	Asl(temp);
	return 0;
}
		
int Cpu::AslZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Asl(temp);
	return 0;
}

int Cpu::AslAbs(){
	uint16_t temp;
	Abs(&temp);
	Asl(temp);
	return 0;
}
		
int Cpu::AslAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	Asl(temp);
	return 0;
}

int Cpu::LsrAA(){
	LsrA(&registerA);
	return 0;
}
		
int Cpu::LsrZpg(){
	uint16_t temp;
	Zpg(&temp);
	Lsr(temp);
	return 0;
}
		
int Cpu::LsrZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Lsr(temp);
	return 0;
}
		
int Cpu::LsrAbs(){
	uint16_t temp;
	Abs(&temp);
	Lsr(temp);
	return 0;
}
		
int Cpu::LsrAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	Lsr(temp);
	return 0;
}

int Cpu::RolAA(){
	RolA(&registerA);
	return 0;
}
		
int Cpu::RolZpg(){
	uint16_t temp;
	Zpg(&temp);
	Rol(temp);
	return 0;
}
		
int Cpu::RolZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Rol(temp);
	return 0;
}
		
int Cpu::RolAbs(){
	uint16_t temp;
	Abs(&temp);
	Rol(temp);
	return 0;
}
		
int Cpu::RolAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	Rol(temp);
	return 0;
}

int Cpu::RorAA(){
	RorA(&registerA);
	return 0;
}
		
int Cpu::RorZpg(){
	uint16_t temp;
	Zpg(&temp);
	Ror(temp);
	return 0;
}
		
int Cpu::RorZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Ror(temp);
	return 0;
}
		
int Cpu::RorAbs(){
	uint16_t temp;
	Abs(&temp);
	Ror(temp);
	return 0;
}
		
int Cpu::RorAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	Ror(temp);
	return 0;
}

int Cpu::BitZpg(){
	uint16_t temp;
	Zpg(&temp);
	Bit(temp);
	return 0;
}
			
int Cpu::BitAbs(){
	uint16_t temp;
	Abs(&temp);
	Bit(temp);
	return 0;
}

int Cpu::CmpIm(){
	CmO(registerA, registerPC++);
	return 0;
}
		
int Cpu::CmpZpg(){
	uint16_t temp;
	Zpg(&temp);
	CmO(registerA, temp);
	return 0;
}
		
int Cpu::CmpZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	CmO(registerA, temp);
	return 0;
}
		
int Cpu::CmpAbs(){
	uint16_t temp;
	Abs(&temp);
	CmO(registerA, temp);
	return 0;
}
		
int Cpu::CmpAbsX(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerX, &temp);
	CmO(registerA, temp);
	return counter;
}

int Cpu::CmpAbsY(){
	uint16_t temp;
	int counter;
	counter = AbsO(registerY, &temp);
	CmO(registerA, temp);
	return counter;
}
		
int Cpu::CmpXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	CmO(registerA, temp);
	return 0;
}
	
int Cpu::CmpIndY(){
	uint16_t temp;
	int counter;
	counter = IndO(registerY, &temp);
	CmO(registerA, temp);
	return counter;
}

int Cpu::CpXIm(){
	CmO(registerX, registerPC++);
	return 0;
}
		
int Cpu::CpXZpg(){
	uint16_t temp;
	Zpg(&temp);
	CmO(registerX, temp);
	return 0;
}
		
int Cpu::CpXAbs(){
	uint16_t temp;
	Abs(&temp);
	CmO(registerX, temp);
	return 0;
}
		
int Cpu::CpYIm(){
	CmO(registerY, registerPC++);
	return 0;
}
		
int Cpu::CpYZpg(){
	uint16_t temp;
	Zpg(&temp);
	CmO(registerY, temp);
	return 0;
}
		
int Cpu::CpYAbs(){
	uint16_t temp;
	Abs(&temp);
	CmO(registerY, temp);
	return 0;
}
		
int Cpu::PhaImp(){
	PhO(registerA);
	return 0;
}
		
int Cpu::PhpImp(){
	StatusRegi(bFlg, true);
	PhO(registerP);
	return 0;
}
		
int Cpu::PlaImp(){
	PlO(&registerA);
	return 0;
}
		
int Cpu::PlpImp(){
	PlO(&registerP);
	return 0;
}
		
int Cpu::JmpAbs(){
	uint16_t temp;
	Abs(&temp);
	registerPC = temp;
	return 0;
}
		
int Cpu::JmpInd(){
	uint16_t temp;
	Ind(&temp);
	registerPC = temp;
	return 0;
}
		
int Cpu::JsrAbs(){
	uint16_t temp;
	Abs(&temp);
	Jsr(temp);
	return 0;
}
	
int Cpu::RtsImp(){
	Rts();
	return 0;
}
		
int Cpu::RtiImp(){
	Rti();
	return 0;
}
	
int Cpu::BccRel(){
	return Bxx(cFlg, false);
}

int Cpu::BcsRel(){
	return Bxx(cFlg, true);
}

int Cpu::BeqRel(){
	return Bxx(zFlg, true);
}
		
int Cpu::BneRel(){
	return Bxx(zFlg, false);
}
		
int Cpu::BvcRel(){
	return Bxx(vFlg, false);
}
		
int Cpu::BvsRel(){
	return Bxx(vFlg, true);
}

int Cpu::BplRel(){
	return Bxx(nFlg, false);
}

int Cpu::BmiRel(){
	return Bxx(nFlg, true);
}
		
int Cpu::SecImp(){
	StatusRegi(cFlg, true);
	return 0;
}
		
int Cpu::ClcImp(){
	StatusRegi(cFlg, false);
	return 0;
}
		
int Cpu::SeiImp(){
	StatusRegi(iFlg, true);
	return 0;
}
	
int Cpu::CliImp(){
	StatusRegi(iFlg, false);
	return 0;
}

int Cpu::SedImp(){
	StatusRegi(dFlg, true);
	return 0;
}
		
int Cpu::CldImp(){
	StatusRegi(dFlg, false);
	return 0;
}
		
int Cpu::ClvImp(){
	StatusRegi(vFlg, false);
	return 0;
}

int Cpu::BrkImp(){
	if (iFlg == false){
		StatusRegi(bFlg, true);
		Brk(0xfffe);
	}
	return 0;
}
		
int Cpu::Nop1(){
	return 0;
}

//以下、非公式オペコード

int Cpu::Kill(){
	irq.irqStr.rst = true;
	return 0;
}

int Cpu::Nop2(){
	++registerPC;
	return 0;
}

int Cpu::Nop3(){
	registerPC += 2;
	return 0;
}

int Cpu::NopAbsX(){
	uint16_t temp;
	return AbsO(registerX, &temp);
}

int Cpu::SLOXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	Asl(temp);
	Ora(temp);
	return 0;
}

int Cpu::SLOIndY(){
	uint16_t temp;
	IndO(registerY, &temp);
	Asl(temp);
	Ora(temp);
	return 0;
}

int Cpu::SLOZpg(){
	uint16_t temp;
	Zpg(&temp);
	Asl(temp);
	Ora(temp);
	return 0;
}

int Cpu::SLOZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Asl(temp);
	Ora(temp);
	return 0;
}

int Cpu::SLOAbs(){
	uint16_t temp;
	Abs(&temp);
	Asl(temp);
	Ora(temp);
	return 0;
}

int Cpu::SLOAbsX(){
	uint16_t temp;
	int dummy;
	dummy = AbsO(registerX, &temp);
	Asl(temp);
	Ora(temp);
	return 0;
}

int Cpu::SLOAbsY(){
	uint16_t temp;
	int dummy;
	dummy = AbsO(registerY, &temp);
	Asl(temp);
	Ora(temp);
	return 0;
}

int Cpu::ANCIm(){
	And(registerPC++);
	StatusRegi(cFlg, CheckBit(registerA, 7));
	return 0;
}

int Cpu::RLAXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	Rol(temp);
	And(temp);
	return 0;
}

int Cpu::RLAIndY(){
	uint16_t temp;
	IndO(registerY, &temp);
	Rol(temp);
	And(temp);
	return 0;
}

int Cpu::RLAZpg(){
	uint16_t temp;
	Zpg(&temp);
	Rol(temp);
	And(temp);
	return 0;
}

int Cpu::RLAZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Rol(temp);
	And(temp);
	return 0;
}

int Cpu::RLAAbs(){
	uint16_t temp;
	Abs(&temp);
	Rol(temp);
	And(temp);
	return 0;
}

int Cpu::RLAAbsX(){
	uint16_t temp;
	int dummy;
	dummy = AbsO(registerX, &temp);
	Rol(temp);
	And(temp);
	return 0;
}

int Cpu::RLAAbsY(){
	uint16_t temp;
	int dummy;
	dummy = AbsO(registerY, &temp);
	Rol(temp);
	And(temp);
	return 0;
}

int Cpu::SREXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	Lsr(temp);
	Eor(temp);
	return 0;
}

int Cpu::SREIndY(){
	uint16_t temp;
	IndO(registerY, &temp);
	Lsr(temp);
	Eor(temp);
	return 0;
}

int Cpu::SREZpg(){
	uint16_t temp;
	Zpg(&temp);
	Lsr(temp);
	Eor(temp);
	return 0;
}

int Cpu::SREZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Lsr(temp);
	Eor(temp);
	return 0;
}

int Cpu::SREAbs(){
	uint16_t temp;
	Abs(&temp);
	Lsr(temp);
	Eor(temp);
	return 0;
}

int Cpu::SREAbsX(){
	uint16_t temp;
	int dummy;
	dummy = AbsO(registerX, &temp);
	Lsr(temp);
	Eor(temp);
	return 0;
}

int Cpu::SREAbsY(){
	uint16_t temp;
	int dummy;
	dummy = AbsO(registerY, &temp);
	Lsr(temp);
	Eor(temp);
	return 0;
}

int Cpu::ALRIm(){
	And(registerPC++);
	LsrA(&registerA);
	return 0;
}

int Cpu::RRAXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	Ror(temp);
	Adc(&registerA, temp);
	return 0;
}

int Cpu::RRAIndY(){
	uint16_t temp;
	IndO(registerY, &temp);
	Ror(temp);
	Adc(&registerA, temp);
	return 0;
}

int Cpu::RRAZpg(){
	uint16_t temp;
	Zpg(&temp);
	Ror(temp);
	Adc(&registerA, temp);
	return 0;
}

int Cpu::RRAZpgX(){
	uint16_t temp;
	ZpgO(registerX, &temp);
	Ror(temp);
	Adc(&registerA, temp);
	return 0;
}

int Cpu::RRAAbs(){
	uint16_t temp;
	Abs(&temp);
	Ror(temp);
	Adc(&registerA, temp);
	return 0;
}

int Cpu::RRAAbsX(){
	uint16_t temp;
	int dummy;
	dummy = AbsO(registerX, &temp);
	Ror(temp);
	Adc(&registerA, temp);
	return 0;
}

int Cpu::RRAAbsY(){
	uint16_t temp;
	int dummy;
	dummy = AbsO(registerY, &temp);
	Ror(temp);
	Adc(&registerA, temp);
	return 0;
}

int Cpu::ARRIm(){
	uint8_t temp;
	And(registerPC++);
	temp = registerP & 0x40;
	RorA(&registerA);
	registerP |= temp; 
	return 0;
}

int Cpu::SAXXInd(){
	uint16_t temp;
	OInd(registerX, &temp);
	StO(temp, registerA & registerX);
	return 0;
}

int Cpu::SAXZpg(){
	uint16_t temp;
	Zpg(&temp);
	StO(temp, registerA & registerX);
	return 0;
}

int Cpu::SAXAbs(){
	uint16_t temp;
	Abs(&temp);
	StO(temp, registerA & registerX);
	return 0;
}

int Cpu::XAAIm(){
	TrO(&registerA, registerX);
	And(registerPC++)