/*
 * data.cpp
 *
 *  Created on: Apr 20, 2018
 *      Author: gotoss08
 */

#include "data.h"

void Data::load_font(std::string font_name, std::string font_path, int font_size) {
	TTF_Font* temp_font = TTF_OpenFont(font_path.c_str(), font_size);
	if (!temp_font) {
		SDL_Log("Unable to load font: %s(%d)\n", font_path.c_str(), font_size);
		successfull_load = false;
	}
	TTF_SetFontHinting(temp_font, 1);
	TTF_SetFontKerning(temp_font, 1);
	fonts_map.insert(std::make_pair(font_name, temp_font));
	std::cout << "loaded font: " << font_name << "(" << temp_font << ")" << std::endl;
}

int Data::Load(std::string data_doc_path) {
	pugi::xml_document data_doc;
	pugi::xml_parse_result doc_parse_result = data_doc.load_file(data_doc_path.c_str());
	SDL_Log("data document load status: %s\n", doc_parse_result.description());
	if (!doc_parse_result) successfull_load = false;

	pugi::xml_node fonts_node = data_doc.child("fonts");
	for (pugi::xml_node font : fonts_node.children()) {
		std::string font_name = font.attribute("name").as_string();
		std::string font_path = font.attribute("path").as_string();
		int font_size = font.attribute("size").as_int();

		load_font(font_name, font_path, font_size);
	}

	pugi::xml_node colors_node = data_doc.child("colors");
	for (pugi::xml_node color : colors_node.children()) {
		std::string color_name = color.attribute("name").as_string();
		Uint8 r = color.attribute("r").as_int();
		Uint8 g = color.attribute("g").as_int();
		Uint8 b = color.attribute("b").as_int();
		Uint8 a = color.attribute("a").as_int();

		colors_map.insert(std::make_pair(color_name, SDL_Color{r, g, b, a}));
	}

	if (successfull_load) return 0;
	else return -1;
}

Data::Data() {}

Data::~Data() {
	for (const auto& kv : fonts_map) TTF_CloseFont(kv.second);
}
