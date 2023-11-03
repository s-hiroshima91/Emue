#ifndef COMMON_HPP
#define COMMON_HPP

#define dispX 1080
#define dispY 1920
#define posX 100
#define posY 100
#define winX 256
#define winY 240
#define magni 3

#define buttonSize 45

#include <stdint.h>

typedef union{
	unsigned int irqInt;
	struct {
		unsigned int nmi: 1;
		unsigned int rst: 1;
		unsigned int irbr: 1;
	} irqStr;
} IRQ;

//16bitに変換
inline uint16_t C2US(uint8_t value){
	return value & 0x00ff; //マイナスの扱いを明確にするため上8桁をマスク
}


//N番目のBit確認の関数
inline bool CheckBit(uint8_t operand, int bitN){
	operand >>= bitN;
	return static_cast<bool>(operand & 0b00000001); 
}

//N番目のBit確認の関数
inline uint8_t CheckBitFF(uint8_t operand, int bitN){
	uint8_t value[2] = {0, 0xff};
	operand >>= bitN;
	return value[operand & 0x01];
}

inline int C2I(uint8_t value){
	return static_cast<int>(value & 0x000000ff);
}



#endif