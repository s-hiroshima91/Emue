#include "cartridge.hpp"
#include <fstream>
#include <iostream>

#define HEADER_SIZE 16
#define P_ROM_BANK_SIZE 16 * 1024
#define C_ROM_BANK_SIZE 8 * 1024
#define RAM_BANK_SIZE 64

void NES10(uint8_t *header, RomFeature *romFeature){
	//PRGROMサイズの検出
	romFeature->pRomSize = static_cast<int>(header[4] & 0xff);
	romFeature->pRomSize *= P_ROM_BANK_SIZE;
	//CHRROMサイズの検出
	romFeature->cRomSize = static_cast<int>(header[5] & 0xff);
	romFeature->cRomSize *= C_ROM_BANK_SIZE;
	
	//PRGRamサイズの取得
	int ram;
	if (CheckBit(header[10], 4)){
		ram = 0;
	}else{
		ram = C2I(header[8]);
		if (ram == 0){
			ram = 8 * 1024;
		}else{
			ram *= 8 * 1024;
		}
	}
	romFeature->pRamSize = ram;

	//PRGNVRamの取得
	romFeature->pNVRamSize = 0;
	
	//CHRRamサイズの取得
	ram = static_cast<int>(header[11] & 0x0f);
	if (header[5] == 0){
		romFeature->cRamSize = C_ROM_BANK_SIZE;
	}else{
		romFeature->cRamSize = 0;
	}
	
	//CHRNVRamの取得
	romFeature->cNVRamSize = 0;
	
	romFeature->flg.mirror = CheckBit(header[6], 0);
	romFeature->flg.battery = CheckBit(header[6], 1);
	romFeature->flg.trainer = CheckBit(header[6], 2);
	romFeature->flg.fourScreen = CheckBit(header[6], 3);
	
	//マッパーの取得
	romFeature->mapper = static_cast<int>(header[6] & 0xf0) >> 4;
	romFeature->mapper |= static_cast<int>(header[7] & 0xf0);
	
	//サブマッパーの設定
	romFeature->subMapper = 0;
	
	//PPUの設定
	romFeature->ppu = 0;

}

void NES20(uint8_t *header, RomFeature *romFeature){
	//PRGROMサイズの検出
	romFeature->pRomSize = static_cast<int>(header[4]);
	
	if ((header[9] & 0x0f) ==0x0f){
		//指数表記の場合
		int temp = romFeature->pRomSize & 0x03;
		temp <<= 1;
		++temp;
		romFeature->pRomSize >>= 2;
		romFeature->pRomSize = 2 << romFeature->pRomSize;
		romFeature->pRomSize *= temp;
	}else{
		//通常のromサイズの場合
		romFeature->pRomSize |= static_cast<int>(header[9] & 0x0f) << 8;
	}
	romFeature->pRomSize *= P_ROM_BANK_SIZE;
		
	//CHRROMサイズの検出
	romFeature->cRomSize = static_cast<int>(header[5]);
	
	if ((header[9] & 0x0f) ==0x0f){
		//指数表記の場合
		int temp = romFeature->cRomSize & 0x03;
		temp <<= 1;
		++temp;
		romFeature->cRomSize >>= 2;
		romFeature->cRomSize = 2 << romFeature->cRomSize;
		romFeature->cRomSize *= temp;
	}else{
		//通常のromサイズの場合
		romFeature->cRomSize |= static_cast<int>(header[9] & 0xf0) << 4; 
	}
	romFeature->cRomSize *= C_ROM_BANK_SIZE;
	
	int ram;
	
	//PRGRamサイズの取得
	ram = static_cast<int>(header[10] & 0x0f);
	if (ram == 0){
		romFeature->pRamSize = 0;
	}else{
		romFeature->pRamSize = RAM_BANK_SIZE << ram;
	}
	
	//PRGNVRamの取得
	ram = static_cast<int>(header[10] & 0xf0);
	ram >>= 4;
	if (ram == 0){
		romFeature->pNVRamSize = 0;
	}else{
		romFeature->pNVRamSize = RAM_BANK_SIZE << ram;
	}
	
	//CHRRamサイズの取得
	ram = static_cast<int>(header[11] & 0x0f);
	if (ram == 0){
		romFeature->cRamSize = 0;
	}else{
		romFeature->cRamSize = RAM_BANK_SIZE << ram;
	}
	
	//CHRNVRamの取得
	ram = static_cast<int>(header[11] & 0xf0);
	ram >>= 4;
	if (ram == 0){
		romFeature->cNVRamSize = 0;
	}else{
		romFeature->cNVRamSize = RAM_BANK_SIZE << ram;
	}
	
	romFeature->flg.mirror = CheckBit(header[6], 0);
	romFeature->flg.battery = CheckBit(header[6], 1);
	romFeature->flg.trainer = CheckBit(header[6], 2);
	romFeature->flg.fourScreen = CheckBit(header[6], 3);
	
	//マッパーの取得
	romFeature->mapper = static_cast<int>(header[6] & 0xf0) >> 4;
	romFeature->mapper |= static_cast<int>(header[7] & 0xf0);
	romFeature->mapper |= static_cast<int>(header[8] & 0x0f) << 8;
	
	//サブマッパーの設定
	romFeature->subMapper = static_cast<int>(header[8] & 0xf0) >> 4;
	
	//PPUの設定
	romFeature->ppu = static_cast<int>(header[12] & 0x03);
	
}


Cartridge::Cartridge(std::string Name)
{
	romFeature = new RomFeature();
	romName = Name;
	ReadHeader();
	
}

void Cartridge::ReadHeader(){
	uint8_t header[HEADER_SIZE];
	void *temp;
	
	temp = static_cast<void*>(header);
	
	std::string path = "./rom/";
	
	path += romName;
	path += ".nes";
	
	std::ifstream ifs(path, std::ios::in | std::ios::binary);
		
	if (!ifs){
		std::cout << "ファイルが開けませんでした。" << std::endl;
		return;
	}
	

	ifs.read(static_cast<char*>(temp), HEADER_SIZE);
	ifs.close();
	ifs.clear();

	//NES2.0の場合
	if ((header[7] & 0x0c) == 0x08){
		NES20(header, romFeature);
	}else{
		NES10(header, romFeature);
	}
}

void Cartridge::ReadRom(){
	void *temp;
	int offset = HEADER_SIZE;
	std::string path = "./rom/";
	
	path += romName;
	path += ".nes";
	
	std::ifstream ifs(path, std::ios::in | std::ios::binary);
	if (!ifs){
		std::cout << "ファイルが開けませんでした。" << std::endl;
		return;
	}
	
	ifs.seekg(offset);
	
	//Trainerの読み込み
	//今のところ未使用
	if (romFeature->flg.trainer){
		trainer = new uint8_t[512];
		temp = static_cast<void*>(trainer);
		offset += 512;
	}else{
		//Trainerがない場合
		trainer = nullptr;
	}
	
	ifs.seekg(offset);
	
	int size;
	//PRGRomの読み込み
	size = romFeature->pRomSize;
	prgRom = new uint8_t[size];
	romFeature->flg.pRom = true;
	temp = static_cast<void*>(prgRom);
	ifs.read(static_cast<char*>(temp), size);
	
	offset += size;
	
	ifs.seekg(offset);
	
	//CHRRomの読み出し
	size = romFeature->cRomSize;
	if(romFeature->cRomSize != 0){
		chrRom = new uint8_t[size];
		romFeature->flg.cRom = true;
		temp = static_cast<void*>(chrRom);
		ifs.read(static_cast<char*>(temp), size);

	}
	
	ifs.close();
	ifs.clear();
}


bool Cartridge::ReadRam(){
	void *temp;
	bool saveDate = false;
	//PRGRamの読み出し
	if(romFeature->pRamSize == 0){
		prgRam = nullptr;
	}else{
		prgRam = new uint8_t[romFeature->pRamSize];
		romFeature->flg.pRam = true;
		temp = static_cast<void*>(prgRam);
		if(romFeature->flg.battery){
			std::string path = "./save/";
	
			path += romName;
			path += ".sav";

			std::ifstream ifs(path, std::ios::in | std::ios::binary);
			if (!ifs){
				std::cout << "セーブデータがありません。" << std::endl;
			}else{
				ifs.read(static_cast<char*>(temp), romFeature->pRamSize);
				ifs.close();
				ifs.clear();
				saveDate = true;
			}
		}
	}
	
	bool flg = false;
	//CHRRamの読み出し
	if(romFeature->cRomSize == 0){
		flg = true;
		if (romFeature->cRamSize == 0){
			romFeature->cRamSize = C_ROM_BANK_SIZE;
		}
	}
	
	if (romFeature->cRamSize == 0){
		chrRam = nullptr;	
	}else{
		chrRam = new uint8_t[romFeature->cRamSize];
		romFeature->flg.cRam = true;
	}
	
	if (flg){
		delete[] chrRom;
		romFeature->flg.cRom = false;
		chrRom = chrRam;
	}
	
	//今のところ実装していないのでNULLPTR
	pNVRam = nullptr;
	cNVRam = nullptr;
	return saveDate;
}

void Cartridge::WriteSaveDate(){
	void *temp;
	std::string path = "./save/";
	
	path += romName;
	path += ".sav";
	
	std::ofstream ofs(path, std::ios::out | std::ios::binary);
	if (!ofs){
		std::cout << "セーブデータを新たに作成します。" << std::endl;
	}
	temp = static_cast<void*>(prgRam);
	ofs.write(static_cast<char*>(temp), romFeature->pRamSize);
	ofs.close();
	ofs.clear();
}


Cartridge::~Cartridge(){
	if (romFeature->flg.pRom){
		delete[] prgRom;
	}
	if (romFeature->flg.trainer){
		delete[] trainer;
	}
	if (romFeature->flg.cRom){
		delete[] chrRom;
	}
	if (romFeature->flg.pRam){
		delete[] prgRam;
	}
	if(romFeature->flg.cRam){
		delete[] chrRam;
	}
	delete romFeature;

}
