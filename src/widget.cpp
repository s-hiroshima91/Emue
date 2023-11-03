#include "widget.hpp"
#include <iostream>

Widget::Widget(std::string str, SDL_Renderer *renderer, int w, int h, SDL_Color FONT, SDL_Color WIDGET, SDL_Color EDGE){
	
	name = str;
	rect.w = w;
	rect.h = h;
	
	fontColor = FONT;
	widgetColor = WIDGET;
	edgeColor = EDGE;
	
	CreateWidget(renderer);

}

void Widget::CreateWidget(SDL_Renderer *renderer){

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, rect.w, rect.h);
	
	if (texture == NULL)
	{
		std::cout<< "SDL_CreateTexture Error" << std::endl;
		return;
	}
	
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, widgetColor.r, widgetColor.g, widgetColor.b, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, edgeColor.r, edgeColor.g, edgeColor.b, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(renderer, NULL);

	int length = name.size();
	if (length == 0){
		SDL_SetRenderTarget(renderer, NULL);
		return;
	}
	
	if (TTF_Init() == -1){
		std::cout << "TTF_Init Error" << std::endl;
		return;
	}
	
	TTF_Font *font = TTF_OpenFont("/system/fonts/Roboto-Regular.ttf", 256);
	if (font == NULL)
	{
		std::cout << "TTF_OpenFont Error" << std::endl;
		return;
	}
	
	SDL_Surface *surface;
	SDL_Texture *textTexture;
	
	surface = TTF_RenderText_Solid(font, name.c_str(), fontColor);
	
	textTexture = SDL_CreateTextureFromSurface(renderer, surface);
	
	int temp;
	if (rect.h < rect.w){
		temp = rect.h >> 3;
	}else{
		temp = rect.w >> 3;
	}
	
	SDL_Rect textRect;
	textRect.w = rect.w - temp;
	textRect.h = rect.h - temp;
	textRect.x = temp >> 1;

	temp = (textRect.h * length) >> 1;
	
	if (textRect.w < temp){
		textRect.h = (textRect.w / length) << 1;
		textRect.y = (rect.h - textRect.h) >> 1;
		
	}else{
		textRect.w = temp;
		textRect.y = textRect.x;
		textRect.x = (rect.w - textRect.w) >> 1;

	}
	
	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
	SDL_DestroyTexture(textTexture);
	SDL_SetRenderTarget(renderer, NULL);
}

void Widget::Drow(SDL_Renderer *renderer, int x, int y){
	rect.x = x;
	rect.y = y;

	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void Button::OnClick(SDL_Event *event, std::function<void(void)> callback){
	if (event->type == SDL_MOUSEBUTTONDOWN){
		SDL_Point point = {event->button.x, event->button.y};
		if (SDL_PointInRect(&point, &rect)){
			callback();
		}
	}
}

bool Button::OnClick(SDL_Point *point){
	return SDL_PointInRect(point, &rect);
}

void TextBox::OnClick(SDL_Event *event, SDL_Renderer *renderer){
	if (event->type == SDL_MOUSEBUTTONDOWN){
		if (SDL_IsTextInputActive()){
			SDL_StopTextInput();
		}else{
			SDL_Point point = {event->button.x, event->button.y};
			if (SDL_PointInRect(&point, &rect)){
				SDL_StartTextInput();
				name = "";
				CreateWidget(renderer);
			}
		}
	}else if (event->type == SDL_TEXTINPUT){
		name += event->text.text;
		CreateWidget(renderer);
	}else if (event->type == SDL_TEXTEDITING){
		std::string temp = name;
		name += event->text.text;
		CreateWidget(renderer);
		name = temp;
	}
}

