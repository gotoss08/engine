/*
 * text.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#include "text.h"

int Text::GenerateCharacterMap(SDL_Renderer* renderer) {
	SDL_Color white = {255,255,255,255};

	for (const auto& kv : data->GetFonts()) {
		cout << "font: " << kv.first << endl;
		map<char, SDL_Texture*> current_font_char_map;
		for (char& ch : CHARACTERS) {
			SDL_Surface* char_surface = TTF_RenderGlyph_Blended(kv.second, ch, white);
			current_font_char_map.insert(make_pair(ch, SDL_CreateTextureFromSurface(renderer, char_surface)));
			SDL_FreeSurface(char_surface);
		}
		chars_map.insert(make_pair(kv.first, current_font_char_map));
		SDL_Log("Generated %s textures for characters in font: %s\n", to_string(CHARACTERS.length()).c_str(), kv.first.c_str());
	}

	if (successfull_load) return 0;
	else return -1;
}

void render_text(SDL_Renderer* renderer, string font_name, int x, int y, string text, SDL_Color color) {

}

void Text::Render(SDL_Renderer* renderer, string font_name, int x, int y, string text, SDL_Color color) {
	map<int,SDL_Color> text_color_map;

	SDL_Color cur_color = color;
	string cur_font_name = font_name;
	TTF_Font* cur_font;
	int minx, maxx, miny, maxy, advance, line_height;

	int cursor_x = x, cursor_y = 0;
	size_t tag_cursor_pos = 0;
	while (true) {
		size_t tag_open_pos = text.find_first_of("{{", tag_cursor_pos); // get open tag in text position
		size_t tag_close_pos = text.find_first_of("}}", tag_open_pos); // get close tag in text position

		size_t tag_next_open_pos = text.find_first_of("{{", tag_open_pos + 2);
		if (tag_next_open_pos < tag_close_pos) tag_open_pos = tag_next_open_pos;

		bool text_contain_tags = tag_open_pos != string::npos; // check if remaining part of text contain tags
		// SDL_Log("text_contain_tags: %d\n", text_contain_tags);
		string to_render_text = text.substr(tag_cursor_pos, text_contain_tags ? tag_open_pos - tag_cursor_pos : text.length() - tag_cursor_pos); // get text after previous close tag
		                                                                                                                                         // and before the next opening tag
		// SDL_Log("to render text: %s\n", to_render_text.c_str());

		cur_font = data->Font(cur_font_name);
		line_height = TTF_FontHeight(cur_font);

		// render text
		for (const char& ch : to_render_text) {
			TTF_GlyphMetrics(cur_font, ch, &minx, &maxx, &miny, &maxy, &advance);
			int char_width = maxx > advance ? maxx : advance;

			SDL_Texture* char_texture = chars_map[cur_font_name][ch];
			SDL_Rect char_rect{cursor_x, y + line_height * cursor_y, char_width, line_height};
			cursor_x += advance;

			SDL_SetTextureColorMod(char_texture, cur_color.r, cur_color.g, cur_color.b);
			SDL_SetTextureAlphaMod(char_texture, cur_color.a);
			SDL_RenderCopy(renderer, char_texture, NULL, &char_rect);
		}

		if (!text_contain_tags) break;
		string tag_str = text.substr(tag_open_pos + 2, tag_close_pos - tag_open_pos - 2); // get text inside tag brackets
		// SDL_Log("tag text: %s\n", tag_str.c_str());

		// SDL_Log("text to erase from string: %s\n", text.substr(tag_open_pos, tag_close_pos - tag_open_pos + 2).c_str());
		text.erase(tag_open_pos, tag_close_pos - tag_open_pos + 2); // remove tag from string
		// SDL_Log("text after manipulations: %s\n", text.c_str());

		tag_cursor_pos = tag_open_pos; // move tag cursor to the point where has previous tag been

		size_t tag_delimiter_pos = tag_str.find("="); // get tag attribute/value delimiter
		if (tag_delimiter_pos == string::npos) {
			if (tag_str == "newline") {
				cursor_y++;
				cursor_x = x;
			}

			// SDL_Log("Could not find delimiter '=' in tag: %s\n", tag_str.c_str());
			continue; // if no delimiter -> invalid tag -> no text manipulation
		}

		string attr_name_str = tag_str.substr(0, tag_delimiter_pos);
		string attr_val_str = tag_str.substr(tag_delimiter_pos + 1, tag_str.size() - tag_delimiter_pos - 1);

		// SDL_Log("tag attribute name: '%s', value: '%s'\n", attr_name_str.c_str(), attr_val_str.c_str());

		if (attr_name_str == "color") {
			vector<int> cc_values;
			size_t color_code_cursor_pos = 0, color_code_pos;
			while ((color_code_pos = attr_val_str.find_first_of(',', color_code_cursor_pos)) != string::npos) {
				string color_code_str = attr_val_str.substr(color_code_cursor_pos, color_code_pos - color_code_cursor_pos);
				// SDL_Log("color code: %s\n", color_code_str.c_str());
				if (is_number(color_code_str)) cc_values.push_back(stoi(color_code_str));
				color_code_cursor_pos = color_code_pos + 1;
			}
			string last_color_code_str = attr_val_str.substr(color_code_cursor_pos);
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

				cur_color = tmp_color;
			}
		} else if (attr_name_str == "color_name") {
			if (attr_val_str == "default") cur_color = color;
			else cur_color = data->Color(attr_val_str);
		} else if (attr_name_str == "font") {
			if (attr_val_str == "default") cur_font_name = font_name;
			else cur_font_name = attr_val_str;
		} else SDL_Log("Could determine tag attribute '%s'\n", attr_name_str.c_str());
	}
}

Text::Text(Data* _data) {
	data = _data;
	padding_left = 0, padding_right = 0, padding_up = 0, padding_down = 0;
	align = Alignment::right;
}

Text::~Text() {
	for (const auto& kv1 : chars_map)
		for (const auto& kv2 : kv1.second) SDL_DestroyTexture(kv2.second);
}

