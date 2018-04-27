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
#include <string>
#include <iostream>
#include <chrono>

#include "config.h"
#include "data.h"
#include "text_renderer.h"
#include "util.h"

class Engine
{
  private:
	static Engine *instance;
	Engine();

	Config *config;
	Data *data;
	TextRenderer *text;

	int width;
	int height;
	SDL_Window *window;
	SDL_Renderer *renderer;

	bool running = true;
	SDL_Event event;

	int fps;
	int ups;

	bool successfull_load = true;

	void update();
	void render();

  public:
	virtual ~Engine();
	static Engine *getInstance();
	int Init(std::string);
	int Loop(int, int);
	SDL_Window *GetWindow() { return window; }
	SDL_Renderer *GetRenderer() { return renderer; }
	bool IsRunning() { return running; }
	int GetFps() { return fps; }
	int GetUps() { return ups; }
};

#endif /* ENGINE_H_ */
