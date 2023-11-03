/*mapperのヘッダー*/

#ifndef MAPPER_HPP
#define MAPPER_HPP

#include "common.hpp"

class Cartridge;

typedef struct{
	int counter = 0;
	uint8_t control = 0x0c;
	uint8_t sRegister = 0;
	uint8_t pBankMode = 0x0c;
	uint8_t cBankMode = 0;
	uint8_t bankNum[9] = {};
		/*mapper1の場合、0:chr1, 1:chr2, 2:prg, 3:pRam*/
		/*mapper4の場合、8:最後から2番目固定*/

} MapperStatus;

class Mapper{

public:
	uint8_t *cpuMap[4];
	uint8_t *ppuMap[8];
	uint8_t ppuTable[0x1000];
	uint8_t *screen[4];
	uint8_t *prgRam;
	int mapperNum;
	

	int irqCounter = 0;
	int irq;
	bool irqEnable;
	bool clearFlg = false;
	IRQ *cpuIrq;

	MapperStatus mapperStatus;

	Mapper(Cartridge *cartridge);
	void MapperCall(uint8_t bankNum, uint16_t addr);
	void MapperIRQ();
	
private:
	int flash = 0;


	void (Mapper::*MapFunc)(uint8_t bankNum, uint16_t addr);

	void Mapper000(uint8_t bankNum, uint16_t addr);
	void Mapper001(uint8_t bankNum, uint16_t addr);
	void Mapper002(uint8_t bankNum, uint16_t addr);
	void Mapper003(uint8_t bankNum, uint16_t addr);
	void Mapper004(uint8_t bankNum, uint16_t addr);
	void Mapper030Bat(uint8_t bankNum, uint16_t addr);
	void Mapper030NoBat(uint8_t bankNum, uint16_t addr);


	//void BankSelect(char bankNum, unsigned short addr);

	Cartridge *crg;

	void PrgBank2(int bank16k, int i);

	void ChrBank2(int bank2k, int i);
	void ChrBank4(int bank4k, int i);
	void ChrBank8(int bank8k);
	
	void OneScreen(int i);
	void VMirror();
	void HMirror();
	void FourScreen();
	
	void mapper001prg(uint8_t num);
	void mapper001chr(uint8_t num1, uint8_t num2);
	
	void SxRom();
	uint8_t (Mapper::*SxRomB)(uint8_t value);
	uint8_t (Mapper::*SxRomC)(uint8_t value);
	uint8_t (Mapper::*SxRomD)(uint8_t value);
	uint8_t (Mapper::*SxRomE)(uint8_t value);
	uint8_t ChrA13(uint8_t value);
	uint8_t ChrA14(uint8_t value);
	uint8_t ChrA15(uint8_t value);
	uint8_t ChrA16(uint8_t value);
	uint8_t PrgA18(uint8_t value);
	uint8_t PrgA13(uint8_t value);
	uint8_t PrgA14(uint8_t value);
	uint8_t Dummy(uint8_t value);

};

#endif