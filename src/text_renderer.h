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

enum TextAlignment
{
    left = 0,
    center = 1,
    right = 2
};

struct CharacterMetrics
{
    int minx;
    int maxx;
    int miny;
    int maxy;
    int advance;
    int width;
    int height;
    int line_height;
    int ascent;
    int descent;
};

class TextRenderer
{
    bool successfull_load = true;

    Config *config;
    Data *data;

    SDL_Renderer *renderer;

    std::string CHARACTERS = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`‘’abcdefghijklmnopqrstuvwxyz{|}~ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙúÛüÝÞßàáâãäå¸æçèéêëìíîïðñòóôõö÷øùûýþÿ ";
    std::map<std::string, std::map<char, SDL_Texture *>> chars_map;
    std::map<std::string, std::map<char, CharacterMetrics>> char_metrics_map;

    TextAlignment text_alignment;
    int text_target_width;
    int padding_left, padding_right, padding_top, padding_bottom;
    bool wordwrap, charwrap;

  public:
    TextRenderer(SDL_Renderer *_renderer, Config *_config, Data *_data);
    virtual ~TextRenderer();

    int GenerateCharacterMap();
    void Render(std::string, int, int, std::string, SDL_Color);

    SDL_Renderer *GetRenderer() { return renderer; }
    void SetRenderer(SDL_Renderer *_renderer) { renderer = _renderer; }

    int GetTextAlignment() { return text_alignment; }
    void SetAlignment(TextAlignment _text_alignment) { text_alignment = _text_alignment; }

    int GetTextTargetWidth() { return text_target_width; }
    void SetTextTargetWidth(int _text_target_width) { text_target_width = _text_target_width; }

    // TODO: write getters&setters for each padding
    void SetPadding(int _pl, int _pr, int _pt, int _pb)
    {
        padding_left = _pl;
        padding_right = _pr;
        padding_top = _pt;
        padding_bottom = _pb;
    }

    bool IsWordwrap() { return wordwrap; }
    void SetWordwrap(bool _wordwrap) { wordwrap = _wordwrap; }

    bool IsCharwrap() { return charwrap; }
    void SetCharwrap(bool _charwrap) { charwrap = _charwrap; }
};

#endif /* TEXT_RENDERER_H_ */
