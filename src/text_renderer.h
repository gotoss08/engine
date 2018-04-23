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

enum Alignment {left = 0, center = 1, right = 2};

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

    int padding_left, padding_right, padding_up, padding_down;
    Alignment align;
public:
    TextRenderer(Config* _config, Data* _data);
    virtual ~TextRenderer();

    int GenerateCharacterMap(SDL_Renderer*);
    void Render(SDL_Renderer*, std::string, int, int, std::string, SDL_Color);

    int GetAlignment() { return align; }
};

#endif /* TEXT_RENDERER_H_ */
