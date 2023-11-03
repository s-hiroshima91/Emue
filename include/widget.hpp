#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <string>
#include <functional>

class Widget{
	public:
	SDL_Texture *texture;
	SDL_Rect rect = {0, 0, 0, 0};
	std::string name;
	
	void CreateWidget(SDL_Renderer *renderer);
	
	public:
	SDL_Color fontColor, widgetColor,  edgeColor;
	
	
	Widget(std::string str, SDL_Renderer *renderer, int w, int h, SDL_Color FONT, SDL_Color WIDGET, SDL_Color EDGE);
	
	void Drow(SDL_Renderer *renderer, int x, int y);
	
	
};

class Button : public Widget{
		public:
		Button(std::string str, SDL_Renderer *renderer, int w, int h) : Widget(str, renderer, w, h, {0x00, 0x00, 0x00, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}){}
		void OnClick(SDL_Event *event, std::function<void(void)> callback);
		bool OnClick(SDL_Point *point);
	
};


class Label : public Widget{
	public:
	Label(std::string str, SDL_Renderer *renderer, int w, int h) : Widget(str, renderer, w, h, {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}){}
	
};

class TextBox : public Label{
	public:
	TextBox(std::string str, SDL_Renderer *renderer, int w, int h) : Label(str, renderer, w, h){}
	
	
	void OnClick(SDL_Event *event, SDL_Renderer *renderer);

};

#endif