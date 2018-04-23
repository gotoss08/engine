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

class Data {
private:
	bool successfull_load = true;

    std::map<std::string, TTF_Font*> fonts_map;
	void load_font(std::string font_name, std::string font_path, int font_size);

    std::map<std::string,SDL_Color> colors_map;
public:
	int Load(std::string data_doc_path);

	TTF_Font* Font(std::string font_name) { return fonts_map[font_name]; }
	std::map<std::string, TTF_Font*> GetFonts() { return fonts_map; }

    SDL_Color Color(std::string color_name) { return colors_map[color_name]; }
    std::map<std::string, SDL_Color> GetColors() { return colors_map; }

    Data();
	~Data();
};

#endif /* DATA_H_ */
