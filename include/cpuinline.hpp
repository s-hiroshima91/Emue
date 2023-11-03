#ifndef CPUINLINE_HPP
#define CPUINLINE_HPP

//ゼロを確認するための関数
inline bool CheckZero(uint8_t operand){
	bool flg = false;
	if (operand == 0){
		flg = true;
	}
	return flg;
}

//16bitアドレスに変換、ただし符号を考慮
inline uint16_t C2S(uint8_t valueC){
	bool flg;
	uint16_t valueS;
	//最上位bitが1なら追加する桁を1で埋める
	flg = CheckBit(valueC, 7);
	if (flg == true){
		valueS = static_cast<uint16_t>(valueC) | 0xff00;
	}else{
		valueS = static_cast<uint16_t>(valueC) & 0x00ff;
	}
	return valueS;
}
	
//2バイト読み込む関数
inline uint16_t PrgCounter(uint8_t *addr){
	return (static_cast<uint16_t>(addr[1]) << 8) + static_cast<uint16_t>(addr[0]);
}

#endif