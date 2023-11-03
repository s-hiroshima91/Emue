#include <iostream>
#include "cpu.hpp"
#include "ioport.hpp"
#include "mapper.hpp"
#include "common.hpp"


/*メモリマップ*/
uint8_t *Cpu::MemoryMap(uint16_t addr){
	uint8_t *pointer;
	
	if (addr < 0x2000){
		addr &= 0x07ff;
		pointer = &cpuRam[addr];

	}else if (addr < 0x4020){
		ioReg = ioPort->IORead(addr);
		pointer = &ioReg;

	}else if (addr < 0x6000){
		std::cout << "ext_rom 未実装" << std::endl;
		pointer = nullptr;
		
	}else if (addr < 0x8000){
		addr -= 0x6000;
		pointer = &mapper->prgRam[addr];//&extRam[addr];

	}else{
		//マッパーのメモリマップに従う
		addr -= 0x8000;
		
		//14,15bitでcpuMapを選ぶ
		pointer = mapper->cpuMap[addr >> 13];
		//1～13bitで値を取得
		pointer += addr & 0x1fff;
	}
	return pointer;
}

/*割り込み*/
int Cpu::Interrupt(){
	if (irq.irqInt == 0){
		return 0;
	}
	bool flg;
	if (irq.irqStr.nmi == true){
		StatusRegi(bFlg, false);
		Brk(0xfffa);
		//irq.irqStr.nmi = false;
		irq.irqInt = 0;
	}else if (irq.irqStr.rst == true){
		registerPC = PrgCounter(MemoryMap(0xfffc));
		StatusRegi(iFlg, true);
		irq.irqStr.rst = false;
	}else if (irq.irqStr.irbr == true){
		flg = CheckBit(registerP, iFlg);
		if (flg){
			return 0;
		}
		
		StatusRegi(bFlg, false);
		Brk(0xfffe);
		irq.irqStr.irbr = false;
	}
	return 7 * 3;
}

/*コンストラクタ*/
Cpu::Cpu(RomFeature *romFeature, Mapper *MAPPER, IOPort *ioP){
	
	irq.irqInt = 0;
	std::cout<<irq.irqStr.rst << std::endl;
	irq.irqStr.rst = true;
	ioPort = ioP;
	ioP->cpuIRQ = &irq;
	mapper = MAPPER;
	mapper->cpuIrq = &irq;
	romSize = romFeature->pRomSize;
	registerP = 0b00100000;
	registerS = 0xfd;
	
	int i;
	i = Interrupt();
	
}

/*メインのシーケンス*/
int Cpu::mainRun(){
	
	int counter;
	/*オペコードを取得*/
	uint8_t opeCode;
	opeCode = *MemoryMap(registerPC++);

	/*メインの処理実行*/
	counter = (this->*cpuInstrs[opeCode])();
	
	counter += cpuCycles[opeCode];
	StatusRegi(rFlg, true);
	counter += Interrupt();
	/*処理にかかったクロック数を返す*/
	return counter;
}


