/*
 * text.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#include "text_renderer.h"

int TextRenderer::GenerateCharacterMap(SDL_Renderer *renderer)
{
	SDL_Color white = {255, 255, 255, 255};

	for (const auto &kv : data->GetFonts())
	{
		std::cout << "font: " << kv.first << std::endl;

		std::map<char, SDL_Texture *> temp_font_char_map;
		std::map<char, CharacterMetrics> temp_char_metrics_map;

		for (char &ch : CHARACTERS)
		{
			SDL_Surface *char_surface = TTF_RenderGlyph_Blended(kv.second, ch, white);
			temp_font_char_map.insert(std::make_pair(ch, SDL_CreateTextureFromSurface(renderer, char_surface)));
			SDL_FreeSurface(char_surface);

			int minx, maxx, miny, maxy, advance;
			TTF_GlyphMetrics(kv.second, ch, &minx, &maxx, &miny, &maxy, &advance);
			int render_width = maxx > advance ? maxx : advance;
			temp_char_metrics_map.insert(std::make_pair(ch, CharacterMetrics{minx, maxx, miny, maxy, advance, render_width}));
		}

		chars_map.insert(std::make_pair(kv.first, temp_font_char_map));
		char_metrics_map.insert(std::make_pair(kv.first, temp_char_metrics_map));

		SDL_Log("Generated %s textures for characters in font: %s\n", std::to_string(CHARACTERS.length()).c_str(), kv.first.c_str());
	}

	if (successfull_load)
		return 0;
	else
		return -1;
}

void TextRenderer::Render(SDL_Renderer *renderer, std::string font_name, int x, int y, std::string text, SDL_Color color)
{
	std::map<int, std::vector<std::string>> tags_map;

	// some ${text and another text

	std::string open_tag = "${";
	std::string close_tag = "}";

	size_t tag_cursor_pos = 0,
		   open_tag_pos,
		   close_tag_pos,
		   next_open_tag_pos;

	while ((open_tag_pos = text.find_first_of(open_tag, tag_cursor_pos)) != std::string::npos)
	{
		tag_cursor_pos = open_tag_pos;

		close_tag_pos = text.find_first_of(close_tag, tag_cursor_pos);

		next_open_tag_pos = text.find_first_of(open_tag, tag_cursor_pos + open_tag.size());

		if (next_open_tag_pos != std::string::npos && next_open_tag_pos <= close_tag_pos)
		{
			open_tag_pos = next_open_tag_pos;
			LOG_F(ERROR, "Tag wasn`t closed: {}", text);
		}

		if (close_tag_pos == std::string::npos)
		{
			LOG_F(ERROR, "Could not find close tag in: {}", text);
			break;
		}

		std::string tag_str = text.substr(open_tag_pos + open_tag.size(), close_tag_pos - open_tag_pos - open_tag.size());
		LOG_F(INFO, "tag: {}", tag_str);

		if (tags_map.find(tag_cursor_pos) != tags_map.end())
			tags_map[tag_cursor_pos].push_back(tag_str);
		else
		{
			std::vector<std::string> temp_vector;
			temp_vector.push_back(tag_str);
			tags_map[tag_cursor_pos] = temp_vector;
		}

		LOG_F(INFO, "tags map now contains {} text tag insert positions", tags_map.size());

		text.erase(open_tag_pos, (close_tag_pos + close_tag.size()) - open_tag_pos);
		LOG_F(INFO, "text after manipulations: {}", text);
	}

	int max_width = stoi(config->Get("window-width")),
		max_height = stoi(config->Get("window-height")),
		cur_width = x,
		cur_height = 0,
		char_index = 0;

	for (const char &ch : text)
	{
		bool newline = false;
		if (tags_map.find(char_index) != tags_map.end()) // there is a tag attached to current character index
		{
			std::vector<std::string> tags_vector = tags_map[char_index];
			for (const std::string &tag_str : tags_vector)
			{
				if (tag_str == "newline")
					newline = true;
			}
		}

		if (newline)
		{
			cur_width = x;
		}

		CharacterMetrics char_metrics = char_metrics_map[font_name][ch];
		cur_width += char_metrics.advance;

		if (cur_width >= max_width - x)
		{
			int new_line_insert_pos = char_index;
			int prev_word_length = 0;

			for (int i = char_index; i >= 0; i--)
			{
				char_metrics = char_metrics_map[font_name][text[i]];
				prev_word_length += char_metrics.advance;
				if (text[i] == ' ')
				{
					new_line_insert_pos = i;
					break;
				}
			}

			cur_width = x + prev_word_length;

			LOG_F(INFO, "text before reaching maximum width: '{}'", text.substr(0, new_line_insert_pos));

			if (tags_map.find(new_line_insert_pos) != tags_map.end())
				tags_map[new_line_insert_pos].push_back("newline");
			else
			{
				std::vector<std::string> temp_vector;
				temp_vector.push_back("newline");
				tags_map[new_line_insert_pos] = temp_vector;
			}
			// text.insert(prev_space_pos, "${newline}");
		}

		char_index++;
	}

	int cursor_x = x,
		cursor_y = 0,
		line_height = TTF_FontHeight(data->Font(font_name));

	cur_width = 0;
	cur_height = 0;
	char_index = 0;

	for (const char &ch : text)
	{
		bool newline = false;
		if (tags_map.find(char_index) != tags_map.end()) // there is a tag attached to current character index
		{
			std::vector<std::string> tags_vector = tags_map[char_index];
			for (const std::string &tag_str : tags_vector)
			{
				if (tag_str.find_first_of("=") == std::string::npos) // statement tag | ${newline}
				{
					LOG_F(INFO, "found statement tag: '{}'", tag_str);
					if (tag_str == "newline")
						newline = true;
				}
				else // control tag | ${color_name=white}
				{
					LOG_F(INFO, "found control tag: '{}'", tag_str);
				}
			}
		}

		char_index++;

		CharacterMetrics char_metrics = char_metrics_map[font_name][ch];

		cur_width += char_metrics.render_width;
		cur_height = line_height * cursor_y;

		if (newline)
		{
			cursor_x = x;
			cursor_y++;
			if (ch != ' ')
				cur_width = char_metrics.advance; // if first character on new line is not 'space' then do nothing
			else
			{
				cur_width = 0;
				continue;
			}
		}

		SDL_Texture *char_texture = chars_map[font_name][ch];
		SDL_Rect char_rect{cursor_x, y + line_height * cursor_y, char_metrics.render_width, line_height};
		cursor_x += char_metrics.advance;

		SDL_SetTextureColorMod(char_texture, color.r, color.g, color.b);
		SDL_SetTextureAlphaMod(char_texture, color.a);
		SDL_RenderCopy(renderer, char_texture, NULL, &char_rect);
	}

	// SDL_Color cur_color = color;
	// std::string cur_font_name = font_name;
	// TTF_Font* cur_font;
	// int minx, maxx, miny, maxy, advance, line_height;
	// int cur_width = 0, cur_height = 0;
	// int max_width = std::stoi(config->Get("window-width")), max_height = std::stoi(config->Get("window-height"));

	// // logger->info("max_width: {}, max_height: {}", max_width, max_height);
	// LOG_F(INFO, "max_width: {}, max_height: {}", max_width, max_height);

	// int cursor_x = x, cursor_y = 0;
	// size_t tag_cursor_pos = 0;
	// while (true) {
	// 	size_t tag_open_pos = text.find_first_of("{{", tag_cursor_pos); // get open tag in text position
	// 	size_t tag_close_pos = text.find_first_of("}}", tag_open_pos); // get close tag in text position

	// 	size_t tag_next_open_pos = text.find_first_of("{{", tag_open_pos + 2);
	// 	if (tag_next_open_pos < tag_close_pos) tag_open_pos = tag_next_open_pos;

	// 	bool text_contain_tags = tag_open_pos != std::string::npos; // check if remaining part of text contain tags
	// 	// SDL_Log("text_contain_tags: %d\n", text_contain_tags);
	// 	std::string to_render_text = text.substr(tag_cursor_pos, text_contain_tags ? tag_open_pos - tag_cursor_pos : text.length() - tag_cursor_pos); // get text after previous close tag
	// 	                                                                                                                                         // and before the next opening tag
	// 	// SDL_Log("to render text: %s\n", to_render_text.c_str());

	// 	cur_font = data->Font(cur_font_name);
	//  line_height = TTF_FontHeight(cur_font);

	// 	// render text
	// for (const char& ch : to_render_text) {
	// 	TTF_GlyphMetrics(cur_font, ch, &minx, &maxx, &miny, &maxy, &advance);
	// 	int char_width = maxx > advance ? maxx : advance;

	// 	cur_width += advance;
	// 	cur_height = line_height * cursor_y;

	// 	if (cur_width >= max_width - x) {
	// 		cursor_x = x;
	// 		cursor_y++;
	// 		cur_width = advance;
	// 	}

	// 	SDL_Texture* char_texture = chars_map[cur_font_name][ch];
	// 	SDL_Rect char_rect{cursor_x, y + line_height * cursor_y, char_width, line_height};
	// 	cursor_x += advance;

	// 	SDL_SetTextureColorMod(char_texture, cur_color.r, cur_color.g, cur_color.b);
	// 	SDL_SetTextureAlphaMod(char_texture, cur_color.a);
	// 	SDL_RenderCopy(renderer, char_texture, NULL, &char_rect);
	// }

	// 	if (!text_contain_tags) break;
	// 	std::string tag_str = text.substr(tag_open_pos + 2, tag_close_pos - tag_open_pos - 2); // get text inside tag brackets
	// 	// SDL_Log("tag text: %s\n", tag_str.c_str());

	// 	// SDL_Log("text to erase from string: %s\n", text.substr(tag_open_pos, tag_close_pos - tag_open_pos + 2).c_str());
	// 	text.erase(tag_open_pos, tag_close_pos - tag_open_pos + 2); // remove tag from string
	// 	// SDL_Log("text after manipulations: %s\n", text.c_str());

	// 	tag_cursor_pos = tag_open_pos; // move tag cursor to the point where has previous tag been

	// 	size_t tag_delimiter_pos = tag_str.find("="); // get tag attribute/value delimiter
	// 	if (tag_delimiter_pos == std::string::npos) {
	// 		if (tag_str == "newline") {
	// 			cursor_y++;
	// 			cursor_x = x;
	// 		}

	// 		// SDL_Log("Could not find delimiter '=' in tag: %s\n", tag_str.c_str());
	// 		continue; // if no delimiter -> invalid tag -> no text manipulation
	// 	}

	// 	std::string attr_name_str = tag_str.substr(0, tag_delimiter_pos);
	// 	std::string attr_val_str = tag_str.substr(tag_delimiter_pos + 1, tag_str.size() - tag_delimiter_pos - 1);

	// 	// SDL_Log("tag attribute name: '%s', value: '%s'\n", attr_name_str.c_str(), attr_val_str.c_str());

	// 	if (attr_name_str == "color") {
	// 		std::vector<int> cc_values;
	// 		size_t color_code_cursor_pos = 0, color_code_pos;
	// 		while ((color_code_pos = attr_val_str.find_first_of(',', color_code_cursor_pos)) != std::string::npos) {
	// 			std::string color_code_str = attr_val_str.substr(color_code_cursor_pos, color_code_pos - color_code_cursor_pos);
	// 			// SDL_Log("color code: %s\n", color_code_str.c_str());
	// 			if (is_number(color_code_str)) cc_values.push_back(stoi(color_code_str));
	// 			color_code_cursor_pos = color_code_pos + 1;
	// 		}
	// 		std::string last_color_code_str = attr_val_str.substr(color_code_cursor_pos);
	// 		if (is_number(last_color_code_str)) cc_values.push_back(stoi(last_color_code_str));

	// 		SDL_Color tmp_color;

	// 		if (cc_values.size() > 2) {
	// 			if (cc_values.size() == 3) {
	// 				tmp_color.r = cc_values[0];
	// 				tmp_color.g = cc_values[1];
	// 				tmp_color.b = cc_values[2];
	// 				tmp_color.a = 255;
	// 			} else {
	// 				tmp_color.r = cc_values[0];
	// 				tmp_color.g = cc_values[1];
	// 				tmp_color.b = cc_values[2];
	// 				tmp_color.a = cc_values[3];
	// 			}

	// 			// SDL_Log("final color: (%d,%d,%d,%d)\n", (int) tmp_color.r, (int) tmp_color.g, (int) tmp_color.b, (int) tmp_color.a);
	// 			cur_color = tmp_color;
	// 		}
	// 	} else if (attr_name_str == "color_name") {
	// 		if (attr_val_str == "default") cur_color = color;
	// 		else cur_color = data->Color(attr_val_str);
	// 	} else if (attr_name_str == "font") {
	// 		if (attr_val_str == "default") cur_font_name = font_name;
	// 		else cur_font_name = attr_val_str;
	// 	} else SDL_Log("Could determine tag attribute '%s'\n", attr_name_str.c_str());
	// }
}

TextRenderer::TextRenderer(Config *_config, Data *_data)
{
	config = _config;
	data = _data;
	padding_left = 0, padding_right = 0, padding_up = 0, padding_down = 0;
	align = Alignment::right;
}

TextRenderer::~TextRenderer()
{
	for (const auto &kv1 : chars_map)
		for (const auto &kv2 : kv1.second)
			SDL_DestroyTexture(kv2.second);
}
