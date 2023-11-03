#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include "common.hpp"
#include <string>

typedef struct {
	int pRomSize;
	int cRomSize;
	int pRamSize;
	int cRamSize;
	int pNVRamSize;
	int cNVRamSize;
	
	struct {
		uint16_t mirror: 1;
		uint16_t battery: 1;
		uint16_t trainer: 1;
		uint16_t fourScreen: 1;
		uint16_t pRom: 1;
		uint16_t pRam: 1;
		uint16_t cRom: 1;
		uint16_t cRam: 1;
		uint16_t pNVRam: 1;
		uint16_t cNVRam: 1;
	} flg;
	
	int mapper;
	int subMapper;
	int ppu;
} RomFeature;

class Cartridge{
	private:
		std::string romName;

		void ReadHeader();
		
	public:
		void ReadRom();
		bool ReadRam();

		RomFeature *romFeature;
		uint8_t *prgRom, *trainer, *chrRom, *prgRam,  *chrRam, *pNVRam, *cNVRam;
		
		Cartridge(std::string Name);
		void WriteSaveDate();
		~Cartridge();
};


#endif