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
public:
	int Load(string data_doc_path);
	TTF_Font* Font(string font_name);
	map<string,TTF_Font*> GetFonts() { return fonts_map; }

    Data();
	~Data();
};

#endif /* DATA_H_ */
