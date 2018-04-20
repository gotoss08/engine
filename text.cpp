/*
 * text.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#include "text.h"

int Text::GenerateCharacterMap(SDL_Renderer* renderer) {
	SDL_Color white = {255,255,255,255};

	for (const auto& kv : fonts_map) {
		cout << "font: " << kv.first << endl;
		map<char, SDL_Texture*> current_font_char_map;
		for (char& ch : CHARACTERS) {
			SDL_Surface* char_surface = TTF_RenderGlyph_Blended(kv.second, ch, white);
			current_font_char_map.insert(make_pair(ch, SDL_CreateTextureFromSurface(renderer, char_surface)));
			SDL_FreeSurface(char_surface);
		}
		chars_map.insert(make_pair(kv.first, current_font_char_map));
		SDL_Log("Generated textures for %s characters for font: %s\n", to_string(CHARACTERS.length()).c_str(), kv.first.c_str());
	}

	if (successfull_load) return 0;
	else return -1;
}

void Text::Render(SDL_Renderer* renderer, string font_name, int x, int y, string text, SDL_Color color) {
	map<int,SDL_Color> text_color_map;

	if (text.find("{{") != string::npos && text.find("}}") != string::npos) {
		size_t tag_cursor_pos = 0, open_tag_pos, close_tag_pos;
		while ((open_tag_pos = text.find_first_of("{{", tag_cursor_pos)) != string::npos) {
			string part_containing_close_tag = text.substr(open_tag_pos+2, text.length());
			tag_cursor_pos = open_tag_pos;

			if ((close_tag_pos = part_containing_close_tag.find_first_of("}}")) != string::npos) {
				string tag_str = text.substr(open_tag_pos+2, close_tag_pos);
				// SDL_Log("tag: %s\n", tag_str.c_str());

				vector<int> cc_values;
				size_t color_code_cursor_pos = 0, color_code_pos;
				while ((color_code_pos = tag_str.find_first_of(',', color_code_cursor_pos)) != string::npos) {
					string color_code_str = tag_str.substr(color_code_cursor_pos, color_code_pos - color_code_cursor_pos);
					if (is_number(color_code_str)) cc_values.push_back(stoi(color_code_str));
					color_code_cursor_pos = color_code_pos + 1;
				}
				string last_color_code_str = tag_str.substr(color_code_cursor_pos);
				if (is_number(last_color_code_str)) cc_values.push_back(stoi(last_color_code_str));

				SDL_Color tmp_color;

				if (cc_values.size() > 2) {
					if (cc_values.size() == 3) {
						tmp_color.r = cc_values[0];
						tmp_color.g = cc_values[1];
						tmp_color.b = cc_values[2];
						tmp_color.a = 255;
					} else {
						tmp_color.r = cc_values[0];
						tmp_color.g = cc_values[1];
						tmp_color.b = cc_values[2];
						tmp_color.a = cc_values[3];
					}
					// SDL_Log("final color: (%d,%d,%d,%d)\n", (int) tmp_color.r, (int) tmp_color.g, (int) tmp_color.b, (int) tmp_color.a);

					text.erase(open_tag_pos, close_tag_pos + 4);
					// SDL_Log("string after manipulations: %s\n", text.c_str());

					text_color_map.insert(make_pair(open_tag_pos, tmp_color));
				}
			}
		}
	}

	SDL_Color current_color = color;

	int current_char_index = 0;
	int cursor_x = x;
	for(char& ch : text) {
		auto font = fonts_map[font_name];
		int minx, maxx, miny, maxy, advance, line_height;
		TTF_GlyphMetrics(font, ch, &minx, &maxx, &miny, &maxy, &advance);
		line_height = TTF_FontHeight(font);

		int char_width = maxx > advance ? maxx : advance;

		SDL_Texture* char_texture = chars_map[font_name][ch];
		SDL_Rect char_rect{cursor_x, y, char_width, line_height};
		cursor_x += advance;

		if (text_color_map.find(current_char_index) != text_color_map.end()) current_color = text_color_map[current_char_index];

		SDL_SetTextureColorMod(char_texture, current_color.r, current_color.g, current_color.b);
		SDL_SetTextureAlphaMod(char_texture, current_color.a);
		SDL_RenderCopy(renderer, char_texture, NULL, &char_rect);

//		SDL_Log("error after rendering text: %s\n", SDL_GetError());

		current_char_index++;
	}
}

Text::~Text() {
	for (const auto& kv1 : chars_map)
		for (const auto& kv2 : kv1.second) SDL_DestroyTexture(kv2.second);
}

