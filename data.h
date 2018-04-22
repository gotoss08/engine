/*
 * data.h
 *
 *  Created on: Apr 20, 2018
 *      Author: gotoss08
 */

#ifndef DATA_H_
#define DATA_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <iostream>
#include <map>
#include "pugixml.hpp"

using namespace std;
using namespace pugi;

class Data {
private:
	bool successfull_load = true;

    map<string,TTF_Font*> fonts_map;
	void load_font(string font_name, string font_path, int font_size);

    map<string,SDL_Color> colors_map;
public:
	int Load(string data_doc_path);

	TTF_Font* Font(string font_name) { return fonts_map[font_name]; }
	map<string,TTF_Font*> GetFonts() { return fonts_map; }

    SDL_Color Color(string color_name) { return colors_map[color_name]; }
    map<string,SDL_Color> GetColors() { return colors_map; }

    Data();
	~Data();
};

#endif /* DATA_H_ */
