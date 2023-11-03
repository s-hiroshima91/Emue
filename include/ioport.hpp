/*IOPortのヘッダー*/

#ifndef IOPORT_HPP
#define IOPORT_HPP

#include "common.hpp"

class Ppu;

class IOPort{


public:
	uint8_t *controller;
	uint8_t padTemp;
	uint8_t padStatus;
	bool dma;
	IRQ *cpuIRQ;

	uint8_t *ioReg[0x30];
	Ppu *ppu;
	uint8_t readBuffer;


	IOPort(Ppu *ppu, uint8_t *key);
	uint8_t IORead(uint16_t addr);
	void IOWrite(uint8_t value, uint16_t addr);
	void DMAFunc(uint8_t *addr);
	
	uint16_t (Ppu::*Ppu::*A12)(uint16_t after);

};

#endif