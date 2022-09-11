#ifndef COMMON_H
#define COMMON_H

//N番目のBit確認の関数
/*inline bool CheckBit(char operand, int bitN);*/


//N番目のBit確認の関数
inline bool CheckBit(char operand, int bitN){
	operand >>= bitN;
	return static_cast<bool>(operand & 0b00000001); 
}

inline int C2I(char value){
	return static_cast<int>(value & 0x000000ff);
}

#endif