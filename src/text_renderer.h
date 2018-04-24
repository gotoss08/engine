/*
 * text.h
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#ifndef TEXT_RENDERER_H_
#define TEXT_RENDERER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "libs/loguru.hpp"

#include "data.h"
#include "config.h"
#include "util.h"

enum TextAlignment {left = 0, center = 1, right = 2};

struct CharacterMetrics {
    int minx;
    int maxx;
    int miny;
    int maxy;
    int advance;
    int render_width;
};

class TextRenderer {
    Config* config;
    Data* data;
    bool successfull_load = true;

    std::string CHARACTERS = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙúÛüÝÞßàáâãäå¸æçèéêëìíîïðñòóôõö÷øùûýþÿ ";
    std::map<std::string, std::map<char, SDL_Texture*>> chars_map;
    std::map<std::string, std::map<char, CharacterMetrics>> char_metrics_map;

    TextAlignment text_alignment;
    int padding_left, padding_right, padding_top, padding_bottom;
    int max_text_rendering_width, max_text_rendering_height;
public:
    TextRenderer(Config* _config, Data* _data);
    virtual ~TextRenderer();

    int GenerateCharacterMap(SDL_Renderer*);
    void Render(SDL_Renderer*, std::string, int, int, std::string, SDL_Color);

    int GetTextAlignment() { return text_alignment; }

    void SetAlignment(TextAlignment _text_alignment) { text_alignment = _text_alignment; }
    void SetPadding(int _pl, int _pr, int _pt, int _pb)
    {
        padding_left = _pl;
        padding_right = _pr;
        padding_top = _pt;
        padding_bottom = _pb;
    }
    void SetTextRenderingBoundaries(int _max_width, int _max_height)
    {
        max_text_rendering_width = _max_width;
        max_text_rendering_height = _max_height;    
    }
};

#endif /* TEXT_RENDERER_H_ */
