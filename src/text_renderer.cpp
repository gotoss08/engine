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
	std::map<int, std::vector<std::string>> tag_map;

	std::string open_tag = "${";
	std::string close_tag = "}";

	size_t tag_cursor_position = 0;
	size_t open_tag_position;

	while ((open_tag_position = text.find_first_of(open_tag, tag_cursor_position)) != std::string::npos)
	{
		tag_cursor_position = open_tag_position;

		size_t close_tag_position = text.find_first_of(close_tag, open_tag_position);
		if (close_tag_position == std::string::npos)
		{
			LOG_F(ERROR, "Could not find closing tag.");
			break;
		}

		size_t next_open_tag_position;
		size_t next_open_tag_cursor_position = open_tag_position + open_tag.size();
		while ((next_open_tag_position = text.find_first_of(open_tag, next_open_tag_cursor_position)) != std::string::npos && next_open_tag_position < close_tag_position)
		{
			LOG_F(ERROR, "Tag inside tag not allowed. Switching current open tag position({}) to {}", open_tag_position, next_open_tag_position);
			open_tag_position = next_open_tag_position;
			next_open_tag_cursor_position = next_open_tag_position + open_tag.size();
		}

		std::string tag_str = text.substr(open_tag_position + open_tag.size(), close_tag_position - (open_tag_position + open_tag.size()));

		if (tag_map.find(tag_cursor_position) != tag_map.end())
			tag_map[tag_cursor_position].push_back(tag_str);
		else
		{
			std::vector<std::string> temp_tag_vector;
			temp_tag_vector.push_back(tag_str);
			tag_map.insert(std::make_pair(tag_cursor_position, temp_tag_vector));
		}

		text.erase(open_tag_position, (close_tag_position + close_tag.size()) - open_tag_position);
	}

	for (auto &tag_pair : tag_map)
	{
		std::string tags_debug_str = "";
		for (const std::string &tag : tag_pair.second)
		{
			tags_debug_str += fmt::format("tag[{}]; ", tag);
		}
		LOG_F(INFO, "tags at index {}: {}", tag_pair.first, tags_debug_str);
	}

	int text_render_cursor = x;
	int line_height;
	for (const char &ch : text) {
		line_height = TTF_FontLineSkip(data->Font(font_name));
		CharacterMetrics character_metrics = char_metrics_map[font_name][ch];

		SDL_Rect char_rect;
		char_rect.x = text_render_cursor;
		char_rect.y = y;
		char_rect.w = character_metrics.render_width;
		char_rect.h = line_height;

		SDL_Texture *char_texture = chars_map[font_name][ch];

		SDL_SetTextureColorMod(char_texture, color.r, color.g, color.b);
		SDL_SetTextureAlphaMod(char_texture, color.a);
		SDL_RenderCopy(renderer, char_texture, NULL, &char_rect);

		text_render_cursor += character_metrics.advance;
	}
}

TextRenderer::TextRenderer(Config *_config, Data *_data)
{
	config = _config;
	data = _data;
	text_alignment = TextAlignment::left;
	padding_left = 0, padding_right = 0, padding_top = 0, padding_bottom = 0;
	max_text_rendering_width = -1, max_text_rendering_height = -1;
}

TextRenderer::~TextRenderer()
{
	for (const auto &kv1 : chars_map)
		for (const auto &kv2 : kv1.second)
			SDL_DestroyTexture(kv2.second);
}
