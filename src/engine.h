/*
 * engine.h
 *
 *  Created on: Apr 16, 2018
 *      Author: gotoss08
 */

#ifndef ENGINE_H_
#define ENGINE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <chrono>
#include <iostream>
#include <string>

#include "libs/loguru.hpp"

#include "config.h"
#include "data.h"
#include "screen.h"
#include "screen_main_menu.h"
#include "text_renderer.h"
#include "util.h"

class Engine {
   private:
    bool successfull_load = true;

    Config *config;
    Data *data;
    TextRenderer *text;

    int width;
    int height;
    SDL_Window *window;
    SDL_Renderer *renderer;

    bool running = true;

    int fps;
    int ups;

    Screen *screen;

   public:
    Engine();
    ~Engine();

    int Init(std::string);
    int Loop(int, int);

    SDL_Window *GetWindow() { return window; }
    SDL_Renderer *GetRenderer() { return renderer; }
    bool IsRunning() { return running; }
    int GetFps() { return fps; }
    int GetUps() { return ups; }

    void SetScreen(Screen *_screen) {
        screen = _screen;
    }
};

#endif /* ENGINE_H_ */
