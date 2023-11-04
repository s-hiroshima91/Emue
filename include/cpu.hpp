/*cpuを実装したクラスのヘッダー*/

#ifndef CPU_HPP
#define CPU_HPP

#include "cartridge.hpp"
#include "cpuinline.hpp"

/*ステータスレジスタの
ビット番号をフラグ名称化*/

#define cFlg 0
#define zFlg 1
#define iFlg 2
#define dFlg 3
#define bFlg 4
#define rFlg 5
#define vFlg 6
#define nFlg 7





class IOPort;
class Mapper;

class Cpu{
private:
	uint8_t registerA, registerX, registerY, registerS, registerP;
	uint16_t registerPC;
	uint8_t ioReg;
	int romSize;
	uint8_t cpuRam[0x800];
	uint8_t *extRam;
	
	Mapper *mapper;
	IOPort *ioPort;
	uint8_t *MemoryMap(uint16_t addr);
	int RMW(uint16_t addr, void (Cpu::*Instrs)(uint8_t*));
	int Operation();

	//ステータスレジスタのbit操作
	//inline関数とする
	void StatusRegi(int sflg, bool status){
		uint8_t flg = 0x01;
		flg <<= sflg;
		if (status == true){
			registerP |= flg;
		}else{
			flg ^= 0xff;
			registerP &= flg;
		}
	}
	
public:
	IRQ irq;
	//bool nmi, rst, irbr;
	
	Cpu(RomFeature *romFeature, Mapper *MAPPER, IOPort *ioP);
	int mainRun();
	int Interrupt();
	//~Cpu();

private:
	/*操作*/
	void TrO(uint8_t *wRegister, uint8_t operand);
	void LdO(uint8_t *wRegister, uint16_t addr);
	int StO(uint16_t wAddr, uint8_t rRegister);
	void AdcBase(uint8_t *wRegister, uint8_t operand);
	void Adc(uint8_t *wRegister, uint16_t addr);
	void Sbc(uint8_t *wRegister, uint16_t addr);
	void And(uint16_t addr);
	void Ora(uint16_t addr);
	void Eor(uint16_t addr);
	void AslA(uint8_t *operand);
	int Asl(uint16_t addr);
	void LsrA(uint8_t *operand);
	int Lsr(uint16_t addr);
	void RolA(uint8_t *operand);
	int Rol(uint16_t addr);
	void RorA(uint8_t *operand);
	int Ror(uint16_t addr);
	void Bit(uint16_t addr);
	void CmO(uint8_t rRegister, uint16_t rMemory);
	void InO(uint8_t *addr);
	int Inc(uint16_t addr);
	void DeO(uint8_t *addr);
	int Dec(uint16_t addr);
	void PhO(uint8_t rRegister);
	void PlO(uint8_t *wRegister);
	void Jmp(uint16_t addr);
	void Jsr(uint16_t addr);
	void Rts();
	void Rti();
	int Bxx(int flg, bool status);
	void Brk(uint16_t addr);

	int DMA();
	
	/*アドレッシング*/
	void Zpg(uint16_t *addr);
	void ZpgO(uint8_t operand, uint16_t *addr);
	void Abs(uint16_t *addr);
	int AbsO(uint8_t operand, uint16_t *addr);
	void OInd(uint8_t operand, uint16_t *addr);
	int IndO(uint8_t operand, uint16_t *addr);
	void Ind(uint16_t *addr);

	
	/*命令*/
	/*0x00*/
	int BrkImp();
	int OraXInd();
	int Kill();//非公式
	int SLOXInd();//非公式
	int Nop2();//非公式
	int OraZpg();
	int AslZpg();
	int SLOZpg();//非公式
	int PhpImp();
	int OraIm();
	int AslAA();
	int ANCIm();//非公式
	int Nop3();//非公式
	int OraAbs();
	int AslAbs();
	int SLOAbs();//非公式
	
	/*0x10*/
	int BplRel();
	int OraIndY();
	//int Kill();//非公式
	int SLOIndY();//非公式
	//int Nop2()//非公式
	int OraZpgX();
	int AslZpgX();
	int SLOZpgX();//非公式
	int ClcImp();
	int OraAbsY();
	//int Nop1();//非公式
	int SLOAbsY();//非公式
	int NopAbsX();//非公式
	int OraAbsX();
	int AslAbsX();
	int SLOAbsX();//非公式
	
	/*0x20*/
	int JsrAbs();
	int AndXInd();
	//int Kill();//非公式
	int RLAXInd();//非公式
	int BitZpg();
	int AndZpg();
	int RolZpg();
	int RLAZpg();//非公式
	int PlpImp();
	int AndIm();
	int RolAA();
	//int ANCIm();//非公式
	int BitAbs();
	int AndAbs();
	int RolAbs();
	int RLAAbs();//非公式
	
	/*0x30*/
	int BmiRel();
	int AndIndY();
	//int Kill();非公式
	int RLAIndY();//非公式
	//int Nop2();//非公式
	int AndZpgX();
	int RolZpgX();
	int RLAZpgX();//非公式
	int SecImp();
	int AndAbsY();
	//int Nop1();//非公式
	int RLAAbsY();//非公式
	//int NopAbsX();//非公式
	int AndAbsX();
	int RolAbsX();
	int RLAAbsX();//非公式
	
	/*0x40*/
	int RtiImp();
	int EorXInd();
	//int Kill();//非公式
	int SREXInd();//非公式
	//int Nop2();//非公式
	int EorZpg();
	int LsrZpg();
	int SREZpg();//非公式
	int PhaImp();
	int EorIm();
	int LsrAA();
	int ALRIm();//非公式
	int JmpAbs();
	int EorAbs();
	int LsrAbs();
	int SREAbs();//非公式
	
	/*0x50*/
	int BvcRel();
	int EorIndY();
	//int Kill();//非公式
	int SREIndY();//非公式
	//int Nop2();//非公式
	int EorZpgX();
	int LsrZpgX();
	int SREZpgX();//非公式
	int CliImp();
	int EorAbsY();
	//int Nop1();//非公式
	int SREAbsY();//非公式
	//int NopAbs();非公式
	int EorAbsX();
	int LsrAbsX();
	int SREAbsX();//非公式
	
	/*0x60*/
	int RtsImp();
	int AdcXInd();
	//int Kill();//非公式
	int RRAXInd();//非公式
	//int Nop2();//非公式
	int AdcZpg();
	int RorZpg();
	int RRAZpg();//非公式
	int PlaImp();
	int AdcIm();
	int RorAA();
	int ARRIm();//非公式
	int JmpInd();
	int AdcAbs();
	int RorAbs();
	int RRAAbs();//非公式	
	
	/*0x70*/
	int BvsRel();
	int AdcIndY();
	//int Kill();//非公式
	int RRAIndY();//非公式
	//int Nop2();//非公式
	int AdcZpgX();
	int RorZpgX();
	int RRAZpgX();//非公式
	int SeiImp();
	int AdcAbsY();
	//int Nop1();非公式
	int RRAAbsY();//非公式
	//int NopAbsX();非公式
	int AdcAbsX();
	int RorAbsX();
	int RRAAbsX();//非公式
	
	/*0x80*/
	//int Nop2();//非公式
	int StaXInd();
	//int Nop2();//非公式
	int SAXXInd();//非公式
	int StyZpg();
	int StaZpg();
	int StxZpg();
	int SAXZpg();//非公式
	int DeYImp();
	//int Nop2();//非公式
	int TxaImp();
	int XAAIm();//非公式
	int StyAbs();
	int StaAbs();
	int StxAbs();
	int SAXAbs();//非公式
	
	/*0x90*/
	int BccRel();
	int StaIndY();
	//int Kill();//非公式
	//非公式
	int StyZpgX();
	int StaZpgX();
	int StxZpgY();
	//非公式
	int TyaImp();
	int StaAbsY();
	int TxsImp();
	//非公式
	//非公式
	int StaAbsX();
	//非公式
	//非公式
	
	/*0xa0*/
	int LdyIm();
	int LdaXInd();
	int LdxIm();
	//非公式
	int LdyZpg();
	int LdaZpg();
	int LdxZpg();
	//非公式
	int TayImp();
	int LdaIm();
	int TaxImp();
	//非公式
	int LdyAbs();
	int LdaAbs();
	int LdxAbs();
	//非公式
	
	/*0xb0*/
	int BcsRel();
	int LdaIndY();
	//非公式
	//非公式
	int LdyZpgX();
	int LdaZpgX();
	int LdxZpgY();
	//非公式
	int ClvImp();
	int LdaAbsY();
	int TsxImp();
	//非公式
	int LdyAbsX();
	int LdaAbsX();
	int LdxAbsY();
	//非公式
	
	/*0xc0*/
	int CpYIm();
	int CmpXInd();
	//非公式
	//非公式
	int CpYZpg();
	int CmpZpg();
	int DecZpg();
	//非公式
	int InYImp();
	int CmpIm();
	int DeXImp();
	//非公式
	int CpYAbs();
	int CmpAbs();
	int DecAbs();
	//非公式
	
	/*0xd0*/
	int BneRel();
	int CmpIndY();
	//非公式
	//非公式
	//非公式
	int CmpZpgX();
	int DecZpgX();
	//非公式
	int CldImp();
	int CmpAbsY();
	//非公式
	//非公式
	//非公式
	int CmpAbsX();
	int DecAbsX();
	//非公式
	
	/*0xe0*/
	int CpXIm();
	int SbcXInd();
	//非公式
	//非公式
	int CpXZpg();
	int SbcZpg();
	int IncZpg();
	//非公式
	int InXImp();
	int SbcIm();
	int Nop1();
	//非公式
	int CpXAbs();
	int SbcAbs();
	int IncAbs();
	//非公式
	
	/*0xf0*/
	int BeqRel();
	int SbcIndY();
	//非公式
	//非公式
	//非公式
	int SbcZpgX();
	int IncZpgX();
	//非公式
	int SedImp();
	int SbcAbsY();
	//非公式
	//非公式
	//非公式
	int SbcAbsX();
	int IncAbsX();
	//非公式	

	int (Cpu::*cpuInstrs[0x100])() ={
		/*0x00*/ &Cpu::BrkImp, &Cpu::OraXInd, &Cpu::Kill, &Cpu::SLOXInd, &Cpu::Nop2, &Cpu::OraZpg, &Cpu::AslZpg, &Cpu::Nop1, &Cpu::PhpImp, &Cpu::OraIm, &Cpu::AslAA, &Cpu::ANCIm, &Cpu::Nop3, &Cpu::OraAbs, &Cpu::AslAbs, &Cpu::SLOAbs,
	/*0x10*/ &Cpu::BplRel, &Cpu::OraIndY, &Cpu::Kill, &Cpu::SLOIndY, &Cpu::Nop2, &Cpu::OraZpgX, &Cpu::AslZpgX, &Cpu::SLOZpgX, &Cpu::ClcImp, &Cpu::OraAbsY, &Cpu::Nop1, &Cpu::SLOAbsY, &Cpu::NopAbsX, &Cpu::OraAbsX, &Cpu::AslAbsX, &Cpu::SLOAbsX,
	/*0x20*/ &Cpu::JsrAbs, &Cpu::AndXInd, &Cpu::Kill, &Cpu::RLAXInd, &Cpu::BitZpg, &Cpu::AndZpg, &Cpu::RolZpg, &Cpu::RLAZpg, &Cpu::PlpImp, &Cpu::AndIm, &Cpu::RolAA, &Cpu::ANCIm, &Cpu::BitAbs, &Cpu::AndAbs, &Cpu::RolAbs, &Cpu::RLAAbs,
	/*0x30*/ &Cpu::BmiRel, &Cpu::AndIndY, &Cpu::Kill, &Cpu::RLAIndY, &Cpu::Nop2, &Cpu::AndZpgX, &Cpu::RolZpgX, &Cpu::RLAZpgX, &Cpu::SecImp, &Cpu::AndAbsY, &Cpu::Nop1, &Cpu::RLAAbsY, &Cpu::NopAbsX, &Cpu::AndAbsX, &Cpu::RolAbsX, &Cpu::RLAAbsX,
	/*0x40*/ &Cpu::RtiImp, &Cpu::EorXInd, &Cpu::Kill, &Cpu::SREXInd, &Cpu::Nop2, &Cpu::EorZpg, &Cpu::LsrZpg, &Cpu::SREZpg, &Cpu::PhaImp, &Cpu::EorIm, &Cpu::LsrAA, &Cpu::ALRIm, &Cpu::JmpAbs, &Cpu::EorAbs, &Cpu::LsrAbs, &Cpu::SREAbs,
	/*0x50*/ &Cpu::BvcRel, &Cpu::EorIndY, &Cpu::Kill, &Cpu::SREIndY, &Cpu::Nop2, &Cpu::EorZpgX, &Cpu::LsrZpgX, &Cpu::SREZpgX, &Cpu::CliImp, &Cpu::EorAbsY, &Cpu::Nop1, &Cpu::SREAbsY, &Cpu::NopAbsX, &Cpu::EorAbsX, &Cpu::LsrAbsX, &Cpu::SREAbsX,
	/*0x60*/ &Cpu::RtsImp, &Cpu::AdcXInd, &Cpu::Kill, &Cpu::RRAXInd, &Cpu::Nop2, &Cpu::AdcZpg, &Cpu::RorZpg, &Cpu::RRAZpg,&Cpu::PlaImp, &Cpu::AdcIm, &Cpu::RorAA, &Cpu::Nop2, &Cpu::JmpInd, &Cpu::AdcAbs, &Cpu::RorAbs, &Cpu::RRAAbs,	
	/*0x70*/ &Cpu::BvsRel, &Cpu::AdcIndY, &Cpu::Kill,&Cpu::RRAIndY, &Cpu::Nop2, &Cpu::AdcZpgX, &Cpu::RorZpgX, &Cpu::RRAZpgX, &Cpu::SeiImp, &Cpu::AdcAbsY, &Cpu::Nop1, &Cpu::RRAAbsY, &Cpu::NopAbsX, &Cpu::AdcAbsX, &Cpu::RorAbsX, &Cpu::RRAAbsX,
	/*0x80*/ &Cpu::Nop2, &Cpu::StaXInd, &Cpu::Nop2, &Cpu::SAXXInd, &Cpu::StyZpg, &Cpu::StaZpg, &Cpu::StxZpg, &Cpu::SAXZpg, &Cpu::DeYImp, &Cpu::Nop2, &Cpu::TxaImp, &Cpu::XAAIm, &Cpu::StyAbs, &Cpu::StaAbs, &Cpu::StxAbs, &Cpu::SAXAbs,
	/*0x90*/ &Cpu::BccRel, &Cpu::StaIndY, &Cpu::Nop1, &Cpu::Nop1, &Cpu::StyZpgX, &Cpu::StaZpgX, &Cpu::StxZpgY, &Cpu::Nop1, &Cpu::TyaImp, &Cpu::StaAbsY, &Cpu::TxsImp, &Cpu::Nop1, &Cpu::Nop1,&Cpu::StaAbsX, &Cpu::Nop1, &Cpu::Nop1,
	/*0xa0*/ &Cpu::LdyIm, &Cpu::LdaXInd, &Cpu::LdxIm, &Cpu::Nop1, &Cpu::LdyZpg, &Cpu::LdaZpg, &Cpu::LdxZpg, &Cpu::Nop1, &Cpu::TayImp, &Cpu::LdaIm, &Cpu::TaxImp, &Cpu::Nop1, &Cpu::LdyAbs, &Cpu::LdaAbs, &Cpu::LdxAbs, &Cpu::Nop1,
	/*0xb0*/ &Cpu::BcsRel, &Cpu::LdaIndY, &Cpu::Nop1, &Cpu::Nop1, &Cpu::LdyZpgX, &Cpu::LdaZpgX, &Cpu::LdxZpgY, &Cpu::Nop1, &Cpu::ClvImp, &Cpu::LdaAbsY, &Cpu::TsxImp, &Cpu::Nop1, &Cpu::LdyAbsX, &Cpu::LdaAbsX, &Cpu::LdxAbsY, &Cpu::Nop1,
	/*0xc0*/ &Cpu::CpYIm, &Cpu::CmpXInd, &Cpu::Nop1, &Cpu::Nop1, &Cpu::CpYZpg, &Cpu::CmpZpg, &Cpu::DecZpg, &Cpu::Nop1, &Cpu::InYImp, &Cpu::CmpIm, &Cpu::DeXImp, &Cpu::Nop1, &Cpu::CpYAbs, &Cpu::CmpAbs, &Cpu::DecAbs, &Cpu::Nop1,
	/*0xd0*/ &Cpu::BneRel, &Cpu::CmpIndY, &Cpu::Nop1, &Cpu::Nop1, &Cpu::Nop1, &Cpu::CmpZpgX, &Cpu::DecZpgX, &Cpu::Nop1, &Cpu::CldImp, &Cpu::CmpAbsY, &Cpu::Nop1, &Cpu::Nop1, &Cpu::Nop1, &Cpu::CmpAbsX, &Cpu::DecAbsX, &Cpu::Nop1,
	/*0xe0*/ &Cpu::CpXIm, &Cpu::SbcXInd, &Cpu::Nop1, &Cpu::Nop1, &Cpu::CpXZpg, &Cpu::SbcZpg, &Cpu::IncZpg, &Cpu::Nop1, &Cpu::InXImp, &Cpu::SbcIm, &Cpu::Nop1, &Cpu::Nop1, &Cpu::CpXAbs, &Cpu::SbcAbs, &Cpu::IncAbs, &Cpu::Nop1,
	/*0xf0*/ &Cpu::BeqRel, &Cpu::SbcIndY, &Cpu::Nop1, &Cpu::Nop1, &Cpu::Nop1, &Cpu::SbcZpgX, &Cpu::IncZpgX, &Cpu::Nop1, &Cpu::SedImp, &Cpu::SbcAbsY, &Cpu::Nop1, &Cpu::Nop1, &Cpu::Nop1, &Cpu::SbcAbsX, &Cpu::IncAbsX, &Cpu::Nop1
	};

int cpuCycles[0x100] = {
	/*0x00*/ 7 * 3, 6 * 3, 2 * 3, 8 * 3, 3 * 3, 3 * 3, 5 * 3, 5 * 3, 3 * 3, 2 * 3, 2 * 3, 2 * 3, 4 * 3, 4 * 3, 6 * 3, 6 * 3,
	/*0x10*/ 2 * 3, 5 * 3, 2 * 3, 8 * 3, 4 * 3, 4 * 3, 6 * 3, 6 * 3, 2 * 3, 4 * 3, 2 * 3, 7 * 3, 4 * 3, 4 * 3, 7 * 3, 7 * 3,
	/*0x20*/ 6 * 3, 6 * 3, 2 * 3, 8 * 3, 3 * 3, 3 * 3, 5 * 3, 5 * 3, 4 * 3, 2 * 3, 2 * 3, 2 * 3, 4 * 3, 4 * 3, 6 * 3, 6 * 3,
	/*0x30*/ 2 * 3, 5 * 3, 2 * 3, 8 * 3, 4 * 3, 4 * 3, 6 * 3, 6 * 3, 2 * 3, 4 * 3, 2 * 3, 7 * 3, 4 * 3, 4 * 3, 7 * 3, 7 * 3,
	/*0x40*/ 6 * 3, 6 * 3, 2 * 3, 8 * 3, 3 * 3, 3 * 3, 5 * 3, 5 * 3, 3 * 3, 2 * 3, 2 * 3, 2 * 3, 3 * 3, 4 * 3, 6 * 3, 6 * 3,
	/*0x50*/ 2 * 3, 5 * 3, 2 * 3, 8 * 3, 4 * 3, 4 * 3, 6 * 3, 6 * 3, 2 * 3, 4 * 3, 2 * 3, 7 * 3, 4 * 3, 4 * 3, 7 * 3, 7 * 3,
	/*0x60*/ 6 * 3, 6 * 3, 2 * 3, 8 * 3, 3 * 3, 3 * 3, 5 * 3, 5 * 3, 4 * 3, 2 * 3, 2 * 3, 2 * 3, 5 * 3, 4 * 3, 6 * 3, 6 * 3,
	/*0x70*/ 2 * 3, 5 * 3, 2 * 3, 8 * 3, 4 * 3, 4 * 3, 6 * 3, 6 * 3, 2 * 3, 4 * 3, 2 * 3, 7 * 3, 4 * 3, 4 * 3, 7 * 3, 7 * 3,
	/*0x80*/ 2 * 3, 6 * 3, 2 * 3, 6 * 3, 3 * 3, 3 * 3, 3 * 3, 3 * 3, 2 * 3, 2 * 3, 2 * 3, 2 * 3, 4 * 3, 4 * 3, 4 * 3, 4 * 3,
	/*0x90*/ 2 * 3, 6 * 3, 2 * 3, 6 * 3, 4 * 3, 4 * 3, 4 * 3, 4 * 3, 2 * 3, 5 * 3, 2 * 3, 5 * 3, 5 * 3, 5 * 3, 5 * 3, 5 * 3,
	/*0xA0*/ 2 * 3, 6 * 3, 2 * 3, 6 * 3, 3 * 3, 3 * 3, 3 * 3, 3 * 3, 2 * 3, 2 * 3, 2 * 3, 2 * 3, 4 * 3, 4 * 3, 4 * 3, 4 * 3,
	/*0xB0*/ 2 * 3, 5 * 3, 2 * 3, 5 * 3, 4 * 3, 4 * 3, 4 * 3, 4 * 3, 2 * 3, 4 * 3, 2 * 3, 4 * 3, 4 * 3, 4 * 3, 4 * 3, 4 * 3,
	/*0xC0*/ 2 * 3, 6 * 3, 2 * 3, 8 * 3, 3 * 3, 3 * 3, 5 * 3, 5 * 3, 2 * 3, 2 * 3, 2 * 3, 2 * 3, 4 * 3, 4 * 3, 6 * 3, 6 * 3,
	/*0xD0*/ 2 * 3, 5 * 3, 2 * 3, 8 * 3, 4 * 3, 4 * 3, 6 * 3, 6 * 3, 2 * 3, 4 * 3, 2 * 3, 7 * 3, 4 * 3, 4 * 3, 7 * 3, 7 * 3,
	/*0xE0*/ 2 * 3, 6 * 3, 2 * 3, 8 * 3, 3 * 3, 3 * 3, 5 * 3, 5 * 3, 2 * 3, 2 * 3, 2 * 3, 2 * 3, 4 * 3, 4 * 3, 6 * 3, 6 * 3,
	/*0xF0*/ 2 * 3, 5 * 3, 2 * 3, 8 * 3, 4 * 3, 4 * 3, 6 * 3, 6 * 3, 2 * 3, 4 * 3, 2 * 3, 7 * 3, 4 * 3, 4 * 3, 7 * 3, 7 * 3
	};
};

#endif