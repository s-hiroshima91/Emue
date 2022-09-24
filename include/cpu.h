/*cpuを実装したクラスのヘッダー*/

#ifndef CPU_H
#define CPU_H

#include "cpuparam.h"

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
public:
	char registerA, registerX, registerY, registerS, registerP;
	unsigned short registerPC;
	
	char* MemoryMap(unsigned short addr);
	
	int mainRun();

	/*命令*/
	void LdO(char *wRegister, char operand);
	void StO(unsigned short wAddr, char rRegister);
	void Adc(char *wRegister, char operand);
	void Sbc(char *wRegister, unsigned short addr);
	void And(unsigned short addr);
	void Ora(unsigned short addr);
	void Eor(unsigned short addr);
	void AslA(char *operand);
	void Asl(unsigned short addr);
	void LsrA(char *operand);
	void Lsr(unsigned short addr);
	void RolA(char *operand);
	void Rol(unsigned short addr);
	void RorA(char *operand);
	void Ror(unsigned short addr);
	void Bit(char operand);
	void CmO(char rRegister, unsigned short rMemory);
	void InO(char *addr);
	void Inc(unsigned short addr);
	void DeO(char *addr);
	void Dec(unsigned short addr);
	void PhO(char rRegister);
	void PlO(char *wRegister);
	void Jmp(unsigned short addr);
	void Jsr(unsigned short addr);
	void Rts();
	void Rti();
	int Bxx(int flg, bool status);
	void Brk(unsigned short addr);

	int DMA();
	
	/*アドレッシング*/
	void Zpg(unsigned short *addr);
	void ZpgO(char operand, unsigned short *addr);
	void Abs(unsigned short *addr);
	int AbsO(char operand, unsigned short *addr);
	void OInd(char operand, unsigned short *addr);
	int IndO(char operand, unsigned short *addr);
	void Ind(unsigned short *addr);

	void StatusRegi(int sflg, bool status);
	int Operation();
	
	enum opeCode opeCode;
	
	bool wFlg = 0;
	
public:
	bool nmi, rst, irbr;
	char cpuRam[0x800];
	char extRam[0x2000] = {};
	char romSize;
	char *rom1;
	char *rom2;
	char *rom3;
	char *rom4;
	char *ppuIO;
	char *padIO;
	IOPort *ioPort;
	Mapper *MP;
	Cpu(char *romDate, char header4, IOPort *ioP, Mapper *Map);
	void Interrupt ();
	//~Cpu();
};

#endif