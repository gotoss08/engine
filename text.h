/*
 * text.h
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#ifndef TEXT_H_
#define TEXT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "util.h"

using namespace std;

class Text {
	bool successfull_load = true;
	string CHARACTERS = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙúÛüÝÞßàáâãäå¸æçèéêëìíîïðñòóôõö÷øùûýþÿ";
	map<string, map<char, SDL_Texture*>> chars_map;
	map<string, TTF_Font*> fonts_map;
public:
	int GenerateCharacterMap(SDL_Renderer*);
	void Render(SDL_Renderer*,string,int,int,string,SDL_Color);

	Text(map<string, TTF_Font*> _fonts_map) { fonts_map = _fonts_map; };
	virtual ~Text();
};

#endif /* TEXT_H_ */
