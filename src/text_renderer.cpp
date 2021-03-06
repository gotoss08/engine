/*
 * text.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#include "text_renderer.h"

TextRenderer::TextRenderer(SDL_Renderer *_renderer, Config *_config, Data *_data) {
    renderer = _renderer;
    config = _config;
    data = _data;
    text_target_width = -1;
    padding_left = 0, padding_right = 0, padding_top = 0, padding_bottom = 0;
    wordwrap = false, charwrap = true;
}

TextRenderer::~TextRenderer() {
    for (const auto &kv1 : chars_map)
        for (const auto &kv2 : kv1.second) SDL_DestroyTexture(kv2.second);
}

int TextRenderer::GenerateCharacterMap() {
    SDL_Color white = {255, 255, 255, 255};

    for (const auto &kv : data->GetFonts()) {
        std::cout << "font: " << kv.first << std::endl;

        std::map<char, SDL_Texture *> temp_font_char_map;
        std::map<char, CharacterMetrics> temp_char_metrics_map;

        for (char &ch : CHARACTERS) {
            SDL_Surface *char_surface = TTF_RenderGlyph_Blended(kv.second, ch, white);
            temp_font_char_map.insert(std::make_pair(ch, SDL_CreateTextureFromSurface(renderer, char_surface)));

            int minx, maxx, miny, maxy, advance, line_height, ascent, descent;
            TTF_GlyphMetrics(kv.second, ch, &minx, &maxx, &miny, &maxy, &advance);
            line_height = TTF_FontLineSkip(kv.second);
            ascent = TTF_FontAscent(kv.second);
            descent = TTF_FontDescent(kv.second);
            temp_char_metrics_map.insert(std::make_pair(ch, CharacterMetrics{minx, maxx, miny, maxy, advance, char_surface->w, char_surface->h, line_height, ascent, descent}));

            SDL_FreeSurface(char_surface);
        }

        chars_map.insert(std::make_pair(kv.first, temp_font_char_map));
        char_metrics_map.insert(std::make_pair(kv.first, temp_char_metrics_map));

        LOG_F(INFO, "Generated {} textures for characters in font: {}", CHARACTERS.length(), kv.first);
    }

    if (successfull_load)
        return 0;
    else
        return -1;
}

RenderedTextMetrics TextRenderer::testRender(std::string font_name, int x, int y, std::string text, SDL_Color color){
    std::string tempText = text;

    struct Character {
        char ch;
        int width;
        int height;
        int render_width;
        int render_height;
        std::string font_name;
        SDL_Color color;
    };

    struct Line {
        std::vector<Character> characters;
        TextAlignment alignment;
        int width;
        int height;
    };

    std::vector<Character> characters;
    std::vector<Line> lines;
    std::map<int, std::vector<std::string>> tag_map;

    std::string open_tag = "${";
    std::string close_tag = "}";

    if (text.find(open_tag) != std::string::npos) {
        size_t cursorPosition = 0, openTagPosition, closeTagPosition;

        while ((openTagPosition = text.find_first_of(open_tag, cursorPosition)) != std::string::npos) {
            closeTagPosition = text.find_first_of(close_tag, openTagPosition + open_tag.size());

            if (closeTagPosition == std::string::npos) {
                LOG_F(ERROR, "Could not find closing tag.");
                break;
            }

            size_t nextOpenTagPosition;
            while ((nextOpenTagPosition = text.find_first_of(open_tag, openTagPosition + open_tag.size())) != std::string::npos && nextOpenTagPosition < closeTagPosition) {
                openTagPosition = nextOpenTagPosition;
            }

            size_t tagStartTextPosition = openTagPosition + open_tag.size();
            std::string tag = text.substr(tagStartTextPosition, closeTagPosition - tagStartTextPosition); // get the tag content

            text.erase(openTagPosition, (closeTagPosition + close_tag.size()) - openTagPosition); // remove tag from text

            cursorPosition = openTagPosition; // move tag cursor to position where have previous tag been

            // insert current tag to tags map at current tag cursor position
            if (tag_map.find(cursorPosition) != tag_map.end()) {
                tag_map[cursorPosition].push_back(tag);
            } else {
                std::vector<std::string> tags;
                tags.push_back(tag);
                tag_map.insert(std::make_pair(cursorPosition, tags));
            }
        }
    }

    // fill characters vector, fetch all caharacter data
    int windowWidth = std::stoi(config->Get("window-width"));
    int maxWidth = text_target_width == -1 ? windowWidth - x - padding_left - padding_right : text_target_width - padding_left - padding_right;

    int startX = x + padding_left;
    int startY = y + padding_top;

    int cursorX = 0;
    int cursorY = 0;

    std::string currentFontName = font_name;
    SDL_Color currentColor = color;

    char ch;
    CharacterMetrics cm;

    for (int i = 0; i < text.length(); i++) {
        ch = text[i];
        cm = char_metrics_map[currentFontName][ch];

        if (tag_map.find(i) != tag_map.end() && !tag_map[i].empty()) {
            for (auto tag : tag_map[i]) {
                if (tag == "newline") {
                    cursorX = 0;
                }
            }
        }

        cursorX += cm.advance;

        // wordwrap = true;
        // LOG_F(INFO, "wordwrapping: {}", wordwrap);

        // check if line width is longer than max width
        if (cursorX > maxWidth && (wordwrap || charwrap)) {
            int insertNewlineIndex = i;
            cursorX = cm.advance;
            cursorY += cm.line_height;

            if (wordwrap) {
                int newlineTextWidth = 0;

                for (int j = i; j >= 0; j--) {
                    ch = text[j];
                    cm = char_metrics_map[currentFontName][ch];

                    newlineTextWidth += cm.advance;

                    if (ch == ' ') {
                        insertNewlineIndex = j;
                        cursorX = newlineTextWidth;
                        break;
                    }
                }
            }

            if (tag_map.find(insertNewlineIndex) != tag_map.end()) {
                tag_map[insertNewlineIndex].push_back("newline");
            } else {
                std::vector<std::string> tags;
                tags.push_back("newline");
                tag_map.insert(std::make_pair(insertNewlineIndex, tags));
            }
        }
    }

    // apply style to characters, fill lines with characters

    TextAlignment currentTextAlignment = text_alignment;
    int currentMaxHeight = 0;

    cursorX = 0;

    std::string tag_delimiter = "=";

    for (int i = 0; i < text.length(); i++) {
        ch = text[i];
        cm = char_metrics_map[currentFontName][ch];

        if (currentMaxHeight < cm.line_height) currentMaxHeight = cm.line_height;

        if (tag_map.find(i) != tag_map.end() && !tag_map[i].empty()) {
            for (auto tag : tag_map[i]) {
                size_t tag_delimiter_position;
                if ((tag_delimiter_position = tag.find_first_of(tag_delimiter)) != std::string::npos) {
                    std::string tag_name = tag.substr(0, tag_delimiter_position);
                    std::string tag_value = tag.substr(tag_delimiter_position + tag_delimiter.size(), tag.size() - (tag_delimiter_position + tag_delimiter.size()));

                    if (tag_name == "color_name") {
                        auto colors = data->GetColors();
                        if (colors.find(tag_value) != colors.end()) {
                            currentColor = colors[tag_value];
                        }
                    } else if (tag_name == "color") {
                        std::vector<int> colorCodes;
                        std::string colorCodeDelimimter = ",";

                        size_t colorCodeDelimimterCursorPosition = 0, colorCodeDelimimterPosition;
                        while ((colorCodeDelimimterPosition = tag_value.find_first_of(colorCodeDelimimter, colorCodeDelimimterCursorPosition)) != std::string::npos) {
                            std::string colorCode = tag_value.substr(colorCodeDelimimterCursorPosition, colorCodeDelimimterPosition - colorCodeDelimimterCursorPosition);

                            if (is_number(colorCode)) colorCodes.push_back(std::stoi(colorCode));

                            colorCodeDelimimterCursorPosition = colorCodeDelimimterPosition + colorCodeDelimimter.size();
                        }

                        std::string lastColorCode = tag_value.substr(colorCodeDelimimterCursorPosition, tag_value.size() - colorCodeDelimimterCursorPosition);
                        if (is_number(lastColorCode)) colorCodes.push_back(std::stoi(lastColorCode));

                        SDL_Color tempColor{255,255,255,255};

                        int colorCodesCount = colorCodes.size();
                        if (colorCodesCount > 2) {
                            if (colorCodesCount == 3) {
                                tempColor.r = colorCodes[0];
                                tempColor.g = colorCodes[1];
                                tempColor.b = colorCodes[2];
                            } else if (colorCodesCount == 4) {
                                tempColor.r = colorCodes[0];
                                tempColor.g = colorCodes[1];
                                tempColor.b = colorCodes[2];
                                tempColor.a = colorCodes[3];
                            }
                        }

                        currentColor = tempColor;
                    }
                } else {
                    if (tag == "newline") {
                        std::vector<Character> tempCharacters(characters);

                        lines.push_back(Line{
                            tempCharacters,
                            currentTextAlignment,
                            cursorX,
                            currentMaxHeight
                        });


                        characters.clear();
                        cursorX = 0;
                    }
                }
            }
        }

        cursorX += cm.advance;

        Character character{
            ch,
            cm.advance,
            cm.line_height,
            cm.width,
            cm.height,
            currentFontName,
            currentColor
        };

        characters.push_back(character);
    }

    // add remaining characters to new line
    if (!characters.empty()) {
        lines.push_back(Line{
            characters,
            currentTextAlignment,
            cursorX,
            currentMaxHeight
        });
    }

    // render created lines

    cursorX = 0;
    cursorY = 0;

    for (const Line &line : lines) {
        // LOG_F(INFO, "line size: {}, alignment: {}, width: {}, height: {}", line.characters.size(), line.alignment, line.width, line.height);
        for (const Character &character : line.characters) {
            // LOG_F(INFO, "character '{}', font: {}, width: {}({}), height: {}({})", character.ch, character.font_name, character.width, character.render_width, character.height, character.render_height);

            if (cursorX == 0 && character.ch == ' ') continue;

            SDL_Rect characterRect{
                startX + cursorX,
                startY + cursorY,
                character.render_width,
                character.render_height
            };

            cursorX += character.width;

            SDL_Texture *char_texture = chars_map[character.font_name][character.ch];
            if (!char_texture || char_texture == NULL) {
                LOG_F(ERROR, "could not find pre-generated texture for character '{}' in font '{}'", ch, font_name);
            }

            SDL_SetTextureColorMod(char_texture, character.color.r, character.color.g, character.color.b);
            SDL_SetTextureAlphaMod(char_texture, character.color.a);
            SDL_RenderCopy(renderer, char_texture, NULL, &characterRect);
        }

        cursorX = 0;
        cursorY += line.height;
    }

    // Render(font_name, x, y + 55, tempText, color);
};

RenderedTextMetrics TextRenderer::Render(std::string font_name, int x, int y, std::string text, SDL_Color color) {
    RenderedTextMetrics renderedTextMetrics;
    renderedTextMetrics.x = x;
    renderedTextMetrics.y = y;

    struct Character {
        int width;
        int height;
        int render_width;
        int render_height;
        std::string font_name;
        SDL_Color color;
    };

    struct Line {
        std::vector<Character> characters;
        TextAlignment alignment;
        int width;
        int height;
    };

    std::vector<Line> lines;

    // tags stuff
    std::map<int, std::vector<std::string>> tag_map;

    std::string open_tag = "${";
    std::string close_tag = "}";

    size_t tag_cursor_position = 0;
    size_t open_tag_position;

    // process all tags from text to map
    while ((open_tag_position = text.find_first_of(open_tag, tag_cursor_position)) != std::string::npos) {
        tag_cursor_position = open_tag_position;

        size_t close_tag_position = text.find_first_of(close_tag, open_tag_position);
        if (close_tag_position == std::string::npos) {
            LOG_F(ERROR, "Could not find closing tag.");
            break;
        }

        size_t next_open_tag_position;
        size_t next_open_tag_cursor_position = open_tag_position + open_tag.size();
        while ((next_open_tag_position = text.find_first_of(open_tag, next_open_tag_cursor_position)) != std::string::npos && next_open_tag_position < close_tag_position) {
            LOG_F(ERROR,
                  "Tag inside tag not allowed. Switching current open tag "
                  "position({}) to {}",
                  open_tag_position, next_open_tag_position);
            open_tag_position = next_open_tag_position;
            next_open_tag_cursor_position = next_open_tag_position + open_tag.size();
        }

        std::string tag_str = text.substr(open_tag_position + open_tag.size(), close_tag_position - (open_tag_position + open_tag.size()));

        if (tag_map.find(tag_cursor_position) != tag_map.end())
            tag_map[tag_cursor_position].push_back(tag_str);
        else {
            std::vector<std::string> temp_tag_vector;
            temp_tag_vector.push_back(tag_str);
            tag_map.insert(std::make_pair(tag_cursor_position, temp_tag_vector));
        }

        text.erase(open_tag_position, (close_tag_position + close_tag.size()) - open_tag_position);
    }

    // calculate text width and add wordwrap tags if needed
    int window_width = stoi(config->Get("window-width"));

    int current_text_width = 0;

    int current_text_max_width = text_target_width == -1 ? window_width - x - padding_left - padding_right : text_target_width - padding_left - padding_right;

    renderedTextMetrics.width = current_text_max_width;

    std::string newline_tag = "newline";

    for (unsigned int i = 0; i < text.size(); i++) {
        char ch = text[i];

        CharacterMetrics character_metrics = char_metrics_map[font_name][ch];

        // if there is alreay a new line tag at this position reset current
        // width
        if (tag_map.find(i) != tag_map.end() && !tag_map[i].empty())
            for (const std::string &tag_str : tag_map[i])
                if (tag_str == newline_tag) current_text_width = 0;

        current_text_width += character_metrics.advance;

        if (current_text_width >= current_text_max_width && (wordwrap || charwrap)) {
            int insert_newline_tag_index = i;
            current_text_width = character_metrics.advance;

            if (wordwrap) {
                int newline_text_width = 0;
                for (int j = i; j >= 0; j--) {
                    ch = text[j];

                    character_metrics = char_metrics_map[font_name][ch];
                    newline_text_width += character_metrics.advance;

                    if (ch == ' ') {
                        insert_newline_tag_index = j;
                        current_text_width = newline_text_width;
                        break;
                    }
                }
            }

            if (tag_map.find(insert_newline_tag_index) != tag_map.end())
                tag_map[insert_newline_tag_index].push_back(newline_tag);
            else {
                std::vector<std::string> temp_tag_vector;
                temp_tag_vector.push_back(newline_tag);
                tag_map.insert(std::make_pair(insert_newline_tag_index, temp_tag_vector));
            }
        }
    }

    // render text
    SDL_Color current_color = color;
    int text_render_cursor_x_begining = x + padding_left;
    int text_render_cursor_x = text_render_cursor_x_begining;
    int line_index = 0;

    for (unsigned int i = 0; i < text.size(); i++) {
        char ch = text[i];

        bool render_this_char = true;

        for (const std::string &tag_str : tag_map[i]) {
            int delimiter_position = tag_str.find_first_of('=');

            if (delimiter_position != std::string::npos) {
                std::string attr_name = tag_str.substr(0, delimiter_position);
                std::string attr_value = tag_str.substr(delimiter_position + 1, tag_str.size() - (delimiter_position + 1));

                if (attr_name == "color_name") {
                    current_color = data->Color(attr_value);
                } else if (attr_name == "color") {
                    LOG_F(INFO, "tag containing color codes: {}", tag_str);

                    std::vector<int> color_codes_vector;
                    int color_delimiter_position, color_delimiter_cursor_position = 0;
                    while ((color_delimiter_position = attr_value.find_first_of(',', color_delimiter_cursor_position)) != std::string::npos) {
                        std::string color_code_str = attr_value.substr(color_delimiter_cursor_position, color_delimiter_position - color_delimiter_cursor_position);
                        if (is_number(color_code_str)) color_codes_vector.push_back(stoi(color_code_str));
                        color_delimiter_cursor_position = color_delimiter_position + 1;
                    }
                    std::string last_color_code_str = attr_value.substr(color_delimiter_cursor_position, attr_value.size() - color_delimiter_cursor_position);
                    if (is_number(last_color_code_str)) color_codes_vector.push_back(stoi(last_color_code_str));

                    for (auto color_code : color_codes_vector) {
                        LOG_F(INFO, "color code extracted from tag: {}", color_code);
                    }

                    SDL_Color temp_color{255, 255, 255, 255};

                    if (color_codes_vector.size() > 2) {
                        if (color_codes_vector.size() == 3) {
                            temp_color.r = color_codes_vector[0];
                            temp_color.g = color_codes_vector[1];
                            temp_color.b = color_codes_vector[2];
                        } else if (color_codes_vector.size() == 4) {
                            temp_color.r = color_codes_vector[0];
                            temp_color.g = color_codes_vector[1];
                            temp_color.b = color_codes_vector[2];
                            temp_color.a = color_codes_vector[3];
                        } else
                            LOG_F(ERROR,
                                  "color tag '{}' does not contains expected "
                                  "count of "
                                  "color code variables",
                                  tag_str);
                    }

                    current_color = temp_color;
                }
            } else {
                if (tag_str == "newline") {
                    text_render_cursor_x = text_render_cursor_x_begining;
                    line_index++;

                    if (ch == ' ') render_this_char = false;
                }
            }
        }

        if (!render_this_char) continue;

        CharacterMetrics character_metrics = char_metrics_map[font_name][ch];

        renderedTextMetrics.height = character_metrics.line_height + line_index * character_metrics.line_height;

        SDL_Texture *char_texture = chars_map[font_name][ch];
        if (!char_texture || char_texture == NULL)
            LOG_F(ERROR,
                  "could not find pre-generated texture for character '{}' in "
                  "font '{}'",
                  ch, font_name);

        SDL_Rect char_rect;
        char_rect.x = text_render_cursor_x;  // + character_metrics.minx
        char_rect.y = y + padding_top + line_index * character_metrics.line_height; // + line_spacing
        char_rect.w = character_metrics.width;
        char_rect.h = character_metrics.height;

        SDL_SetTextureColorMod(char_texture, current_color.r, current_color.g, current_color.b);
        SDL_SetTextureAlphaMod(char_texture, current_color.a);
        SDL_RenderCopy(renderer, char_texture, NULL, &char_rect);

        text_render_cursor_x += character_metrics.advance;
    }

    return renderedTextMetrics;
}
