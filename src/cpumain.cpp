#include "cpu.hpp"
#include "cpuinline.hpp"
#include "cartridge.hpp"
#include <iostream>
#include <fstream>

void Boot(char *rom, ushort *registerPC, char *bank){
	std::ifstream ifs("./cgb_boot.bin", std::ios::in | std::ios::binary);
	if (!ifs){
		std::cout << "ファイルが開けませんでした。" << std::endl;
		*registerPC = 0x0100;
		*bank = 0x01;
		return;
	}

	ifs.read(rom, 0x100 * sizeof(char));
	ifs.seekg(0x200);
	ifs.read(rom + 0x200, 0x700 * sizeof(char));
	ifs.close();

}

void Cpu::BootFin(){
	rom1 = Crg->rom;

}

Cpu::Cpu(Cartridge *cartridge){
	
	Crg = cartridge;
	Crg->Cpu = this;
	rom1 = Crg->boot;
	rom2 = &Crg->rom[romBankSize * 1];
	sRam = Crg->ram;
	wRam1 = new char [0x1000];
	std::fill(wRam1, wRam1 + 0x1000, 0);
	wRamBank = new char [0x7000];
	std::fill(wRamBank, wRamBank + 0x7000, 0);
	wRam2 = wRamBank;
	vRamColor = new char [0x4000];
	std::fill(vRamColor, vRamColor + 0x4000, 0);

	vRam = vRamColor;
	ioReg[0x06] = 0x00;
	ioReg[0x08] = 0xf8;
	ioReg[0x50] = 0x00;
	ioReg[0x40] = 0x91;
	ioReg[0x55] = 0xff;
		
	registerSP = 0xfffe;
	
	registerPC = 0x0000;
	Boot(rom1, &registerPC, &ioReg[0x50]);

}

char *Cpu::MemoryMap(ushort addr){
	char *ptr;
	
	if (addr < 0x4000){
		ptr = &rom1[addr];
		
	}else if (addr < 0x8000){
		addr -= 0x4000;
		ptr = &rom2[addr];
		
	}else if (addr < 0xa000){
		addr -= 0x8000;
		ptr = &vRam[addr];
		
	}else if (addr < 0xc000){
		if (Crg->ramStat == 0){
			addr -= 0xa000;
			ptr = &sRam[addr];
		}else if (Crg->ramStat < 0xff00){
			ptr = &sRam[0];
		}else{
			ptr = &Crg->rtcReg[0x0e];
		}
		
	}else if (addr < 0xd000){
		addr -= 0xc000;
		ptr = &wRam1[addr];
		
	}else if (addr < 0xe000){
		addr -= 0xd000;
		ptr = &wRam2[addr];
		
	}else if (addr >= 0xfe00){
		addr -= 0xe000;
		ptr = &etcRam[addr];

	}else{
		addr -= 0x2000;
		ptr = MemoryMap(addr);
	}
	return ptr;
}

int Cpu::Interrupt(){
	if (ime){
		char flg;
		flg = IE & IF & 0x1f;
		if (flg == 0){
			return 0;
		}
		for (int i = 0; i < 5; ++i){
			if ((flg & 0x01) == 0x01){
				if (halt){
					++registerPC;
					halt = false;
				}
				Push16(registerPC);
				registerPC = 0x0040 + (i << 3);
				IF &= (0x01 << i) ^ 0xff;
				ime = false;
				return 20;
			}else{
				flg >>= 1;
			}
		}
	}
	return 0;
}

void Cpu::Timer(){
	ushort temp;
	
	clockCounter += 4;
	temp = clockCounter;
	temp >>= timerStep[ioReg[0x07] & 0x03];
	temp &= (static_cast<ushort>(ioReg[0x07] >> 2) & 0x01);
	ioReg[0x04] = static_cast<char>(clockCounter >> 8);
	
	if (preTimer == 1){
		IF |= 0x04;
		ioReg[0x05] = ioReg[0x06];
		nextTimer = ioReg[0x05] + 1;
		preTimer = 2;

	}else if(preTimer == 2){
		preTimer = 0;
	}
	
	
	
	preMux &= (temp ^ 0xffff);
	
//	if ((temp & 0x01) == 0x01){
	if (preMux){
		if (ioReg[0x05] == 0xff){
			preTimer = 1;
		}
		ioReg[0x05] = nextTimer;
		++nextTimer;
//		++ioReg[0x05];

	}
	preMux = temp;
	
	
}

int Cpu::Sequence(){
	int counter;
	int i = 0;
	ushort opeCode;
	counter = Interrupt();

	while (i < counter){
		Timer();
		i += 4;
	}

	opeCode = C2US(Imm());

	counter += (this->*cpuInstrs[opeCode])();
	counter += cpuCycle[opeCode];

	while (i < counter){
		Timer();
		i += 4;
	}
	return counter >> doubleSpeed;
}

int Cpu::IOReg(ushort addr, char value){
	addr &= 0x00ff;
	int counter = 0;
	
	if (addr == 0x00){
		if ((value & 0x30) == 0x20){
			ioReg[0x00] = key[0];
			ioReg[0x00] |= 0xe0;
		}else if ((value & 0x30) == 0x10){
			ioReg[0x00] = key[1];
			ioReg[0x00] |= 0xd0;
		}else if ((value & 0x30) == 0x30){
			ioReg[0x00] = 0xff;
		}
		
	}else if (addr == 0x04){
		clockCounter = 0xfffc;
		
	}else if (addr == 0x05){
		if (preTimer == 0){
			ioReg[0x05] = value;
			nextTimer = value + 1;
		}else if (preTimer == 1){	
			preTimer = 0;
			ioReg[0x05] = value;
			nextTimer = value + 1;
		}
		
	}else if (addr == 0x06){
		nextTimer = value;
		ioReg[0x06] = value;
		
	}else if (addr == 0x41){
		ioReg[0x41] &= 0x07;
		ioReg[0x41] |= (value | 0x80) & 0xf8;
		
	}else if (addr == 0x46){
		char *dma;
		ioReg[0x46] = value;
		addr = static_cast<ushort>(value) << 8;
		dma = MemoryMap(addr);
		for (int i = 0; i < 0xa0; ++i){
			oam[i] = dma[i];
		}
		
	}else if (addr == 0x4d){
		ioReg[0x4d] &= 0x80;
		ioReg[0x4d] |= value & 0x01;
		
	}else if (addr == 0x4f){
		value |= 0xfe;
		ioReg[0x4f] = value;
		if (value == 0xfe){
			vRam = vRamColor;
		}else{
			vRam = vRamColor + 0x2000;
		}
		
	}else if (addr == 0x50){
		ioReg[0x50] = value;
		if (!CheckBit(rom1[0x143], 7)){
			**palette[0] = 0x01;
			*(*palette[0] + 1) = 0xf0;
			*palette[0] = &ioReg[0x47];
			*palette[1] = &ioReg[0x48];
		}
		BootFin();
		
	}else if (addr == 0x51){
		hdmaS &= 0x00ff;
		hdmaS |= C2US(value) << 8;
		
	}else if (addr == 0x52){
		hdmaS &= 0xff00;
		hdmaS |= static_cast<ushort>(value) & 0x00f0;
		
	}else if (addr == 0x53){
		hdmaT &= 0x80ff;
		hdmaT |= static_cast<ushort>(value & 0x1f) << 8;
		
	}else if (addr == 0x54){
		hdmaT &= 0xff00;
		hdmaT |= static_cast<ushort>(value) & 0x00f0;
		
	}else if (addr == 0x55){
		if (CheckBit(ioReg[0x55], 7)){
			if (!CheckBit(value, 7)){
				addr = (C2US(value) + 1) << 4;
				for (int i = 0; i < addr; ++i){
					*MemoryMap(hdmaT++) = *MemoryMap(hdmaS++);
				}
				ioReg[0x55] = 0xff;
				counter = (addr << 1) << doubleSpeed;
			
			}else{
				ioReg[0x55] = value & 0x7f;
			}
		}else{
			if (!CheckBit(value, 7)){
				ioReg[0x55] |= 0x80;
			}
		}

	}else if (addr == 0x68){
		ioReg[0x68] = value & 0xbf;
		addr = static_cast<ushort>(value) & 0x3f;
		ioReg[0x69] = bgPalette[addr];
		
	}else if (addr == 0x69){
		Palette(value, static_cast<ushort>(ioReg[0x68] & 0x3f) | 0x40, bgPalette);
		if (CheckBit(ioReg[0x68], 7)){
			++ioReg[0x68];
			ioReg[0x68] &= 0xbf;
		}
		ioReg[0x69] = bgPalette[C2US(ioReg[0x68])];
		
	}else if (addr == 0x6a){
		ioReg[0x6a] = value & 0xbf;
		addr = static_cast<ushort>(value) & 0x3f;
		ioReg[0x6b] = spritePalette[addr];
		
	}else if (addr == 0x6b){
		Palette(value, static_cast<ushort>(ioReg[0x6a] & 0x3f), spritePalette);
		if (CheckBit(ioReg[0x6a], 7)){
			++ioReg[0x6a];
			ioReg[0x6a] &= 0xbf;
		}
		ioReg[0x6b] = spritePalette[C2US(ioReg[0x6a])];
		
	}else if (addr == 0x70){
		addr = static_cast<ushort>(value) & 0x07;
		ioReg[0x70] = 0xf8 | value;
		
		if (addr != 0){
			--addr;
		}
		addr <<= 12;
		wRam2 = &wRamBank[addr];
		
	}else{
		ioReg[addr] = value;
	}
	return counter;

}

int Cpu::HDMA(){
	int i, counter;
	if (CheckBit(ioReg[0x55], 7)){
		return 0;
	}
	
	counter = Interrupt();
	counter >>= doubleSpeed;

	for (i = 0; i < 0x10; ++i){
		*MemoryMap(hdmaT++) = *MemoryMap(hdmaS++);
	}
	if (ioReg[0x55] == 0){
		ioReg[0x55] = 0xff;
	}else{
		--ioReg[0x55];
	}
	counter += 32;
	
	i = counter;
	while(i > 0){
		Timer();
		i -= 2;
	}
	
	return counter;
}

void Cpu::Palette(char value, ushort addr, char *palette){
	char temp = value;
	palette[addr & 0x3f] = value;
	if (!CheckBit(addr, 0)){
		addr >>= 1;
		value &= 0x1f;
		color[addr].color[2] = (value << 3) | (value >> 2);
		value = (temp >> 5) & 0x07;
		color[addr].color[1] &= 0xc6;
		color[addr].color[1] |= (value << 3) | (value >> 2);

	}else{
		addr >>= 1;
		value &= 0x03;
		color[addr].color[1] &= 0x39;
		color[addr].color[1] |= (value << 6) | (value << 1);
		value = temp & 0x7c;
		color[addr].color[0] = (value << 1) | (value >> 4);

	}
}
	

Cpu::~Cpu(){
	
	delete[] wRam1;
	delete[] wRamBank;
	delete[] vRamColor;
}