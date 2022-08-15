/*コントローラのソース*/

#include "controller.h"
#include "common.h"
#include <iostream>

/*"コントローラのオブジェクトとなるクラス*/
Controller::Controller(int posX, int posY, int winX, int winY, int magni){

	upBtn.w = btnSize * magni;
	upBtn.h =  (upBtn.w * 3) >> 1;
	upBtn.x = posX + upBtn.h;
	upBtn.y = posY + winY * magni +btnSize * magni;
	
	leftBtn.h = upBtn.w;
	leftBtn.w = upBtn.h;
	leftBtn.x = upBtn.x - leftBtn.w;
	leftBtn.y = upBtn.y + upBtn.h;

	downBtn.x = upBtn.x;
	downBtn.y = upBtn.y + upBtn.h + leftBtn.h;
	downBtn.w = upBtn.w;
	downBtn.h = upBtn.h;

	rightBtn.x = upBtn.x + upBtn.w;
	rightBtn.y = leftBtn.y;
	rightBtn.w = leftBtn.w;
	rightBtn.h = leftBtn.h;
	
	selBtn.x = rightBtn.x + btnSize * magni * 2;
	selBtn.y = rightBtn.y + btnSize * magni;
	selBtn.w = btnSize * magni;
	selBtn.h = selBtn.w >> 1;
	
	stBtn.x = selBtn.x + selBtn.w + btnSize * magni;
	stBtn.y = selBtn.y;
	stBtn.w = selBtn.w;
	stBtn.h = selBtn.h;
	
	bBtn.x = stBtn.x + btnSize * magni * 2;
	bBtn.y = rightBtn.y;
	bBtn.w = btnSize * magni;
	bBtn.h = bBtn.w;
	
	aBtn.x = bBtn.x + bBtn.w + btnSize *
	magni;
	aBtn.y = bBtn.y;
	aBtn.w = bBtn.w;
	aBtn.h = bBtn.h;

}

/*ボタンが押されたことを判定するクラス*/
char Controller::InputDown(int x, int y, SDL_FingerID id){
	char flg = 0; //押したボタンに対応したビットを立てるフラグ
	int i = 0;
	
	/*押しているボタンを管理する構造体の空きを探す*/
	while(fingerID[i].status == true){
		i += 1;
	}

	/*押した位置からボタンを判定する*/
	if ((x > upBtn.x) && (x < (upBtn.x + upBtn.w))){
		if ((y > upBtn.y) && ((y < upBtn.y + upBtn.h))){
			flg = 0b00010000;
		}else if((y > downBtn.y) && (y < (downBtn.y + downBtn.h))){
			flg = 0b00100000;
		}
		/*フラグと押した指のidを構造体に書き込み
構造体の状態を占有にする*/
		fingerID[i].key = flg;
		fingerID[i].status = true;
		fingerID[i].id = id;
		return flg;
	}
	
	if ((y > leftBtn.y) && (y < (leftBtn.y + leftBtn.h))){
		if ((x > leftBtn.x) && (x < (leftBtn.x + leftBtn.w))){
			flg = 0b01000000;
		}else if ((x > rightBtn.x) && (x < (rightBtn.x + rightBtn.w))){
			flg = 0b10000000;
		}else if ((x > bBtn.x) && (x < (bBtn.x + bBtn.w))){
			flg = 0b00000010;
		}else if ((x > aBtn.x) && (x < (aBtn.x + aBtn.w))){
			flg = 0b00000001;
		}
		fingerID[i].key = flg;
		fingerID[i].status = true;
		fingerID[i].id = id;
		return flg;
	}
	
	if ((y > selBtn.y) && (y < (selBtn.y + selBtn.h))){
		if ((x > selBtn.x) && (x < (selBtn.x + selBtn.w))){
			flg = 0b00000100;
		}else if ((x > stBtn.x) && (x < (stBtn.x + stBtn.w))){
			flg = 0b00001000;
		}
		fingerID[i].key = flg;
		fingerID[i].status = true;
		fingerID[i].id = id;
		return flg;
	}
	return flg;
}

/*放したボタンを判定するクラス*/
char Controller::InputUp(SDL_FingerID id){
	int i = 0;
	
	/*放した指のidを探す*/
	while(id != fingerID[i].id){
		i += 1;
		/*該当するidがなければなにもしない*/
		if (i == 3){
			return 0xff;
		}
	}
	/*構造体の占有を解放する*/
	fingerID[i].status = false;
	/*対応したボタンのビットを0にするため補数をとる*/
	return fingerID[i].key ^ 0xff;
}
/*コントローラを表示するクラス*/
void Controller::Img(SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer, 0x88, 0x88, 0x88, SDL_ALPHA_OPAQUE);

	/*■をバッファに描画*/
    SDL_RenderFillRect(renderer, &aBtn);
    SDL_RenderFillRect(renderer, &bBtn);
    SDL_RenderFillRect(renderer, &selBtn);
    SDL_RenderFillRect(renderer, &stBtn);
    SDL_RenderFillRect(renderer, &upBtn);
    SDL_RenderFillRect(renderer, &downBtn);
    SDL_RenderFillRect(renderer, &rightBtn);
    SDL_RenderFillRect(renderer, &leftBtn);
}


