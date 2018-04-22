/*
 * engine.cpp
 *
 *  Created on: Apr 16, 2018
 *      Author: gotoss08
 */

#include "engine.h"

int Engine::Init(string title) {
	config.Load();

	Engine::width = is_number(config["window-width"]) ? stoi(config["window-width"].c_str()) : 640;
	Engine::height = is_number(config["window-height"]) ? stoi(config["window-height"].c_str()) : 480;

	/* sdl2 lib initialization */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
		successfull_load = false;
	}

	/* sdl2_ttf lib initialization */
	if (TTF_Init() != 0) {
		SDL_Log("Unable to initialize SDL_TTF: %s\n", TTF_GetError());
		successfull_load = false;
	}

	data = new Data();
	if (data->Load(config["data-file"]) != 0) {
		SDL_Log("Unable to load data");
		successfull_load = false;
	}

	/* window creating */
	Uint32 window_resizable_flag = config["resizable"] == "true" ? SDL_WINDOW_RESIZABLE : 0;
	Uint32 window_fullscreen_flag = config["display-mode"] == "fullscreen" ? SDL_WINDOW_FULLSCREEN : 0;
	Uint32 window_borderless_flag = config["display-mode"] == "borderless" ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | window_resizable_flag | window_fullscreen_flag | window_borderless_flag);
	if (window == NULL) {
		SDL_Log("Unable to create window: %s\n", SDL_GetError());
		successfull_load = false;
	}

	/* renderer creation */
	Uint32 renderer_flags;

	if (config["renderer-acceleration"] == "software") renderer_flags = SDL_RENDERER_SOFTWARE;
	else if (config["vsync"] == "on") renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	else renderer_flags = SDL_RENDERER_ACCELERATED;

	renderer = SDL_CreateRenderer(window, -1, renderer_flags);
	if (renderer == NULL) {
		SDL_Log("Unable to create renderer: %s\n", SDL_GetError());
		successfull_load = false;
	}

	/* character maps generation */
	text = new Text(data);
	if (text->GenerateCharacterMap(renderer)) {
		SDL_Log("Unable to generate characters map.");
		successfull_load = false;
	}

	/* if no errors proceed */
	if (successfull_load) return 0;
	else return -1;
}

int Engine::Loop(int _fps, int _ups) {
	// clock
	Engine::fps = _fps;
	Engine::ups = _ups;

	using clock = high_resolution_clock;
	nanoseconds lag(0);
	auto time_start = clock::now();
	auto timer = clock::now();

	int ticks = 0;
	int frames = 0;

	Timer engine_text_rendering_timer;
	Timer sdl_text_rendering_timer;

	string fps_text = "";

	while (running) {
		// calcualte awaited timesteps
		nanoseconds frame_timestep(1000000000 / fps);
		nanoseconds tick_timestep(1000000000 / ups);

		auto delta_time = clock::now() - time_start;
		time_start = clock::now();
		lag += duration_cast<nanoseconds>(delta_time);

		while (lag >= tick_timestep) {
			lag -= tick_timestep;
			update();
			ticks++;
		}

		SDL_SetRenderDrawColor(renderer, 83, 119, 122, 255);
		SDL_RenderClear(renderer);

		render();

		frames++;

		auto frame_render_time = duration_cast<nanoseconds>(clock::now() - time_start);

		if (duration_cast<seconds>(clock::now() - timer).count() >= 1) {
			SDL_Log("frames: %d, ticks: %d | per second\n", frames, ticks);
			fps_text = to_string(frames) + "{{text_color=white}}fps | {{color=2,25,255,1}}" + to_string(duration_cast<milliseconds>(frame_render_time).count()) + "{{color=236,208,12,5}}ms frame render time";

			timer = clock::now();
			ticks = 0;
			frames = 0;
		}

		SDL_Color fps_color = {236,208,120,255};
		text->Render(renderer, "debug_font", 7, 7, fps_text, fps_color);

		SDL_RenderPresent(renderer);

		if (frame_timestep > frame_render_time) {
			SDL_Delay(duration_cast<milliseconds>(frame_timestep - frame_render_time).count());
		}
	}
	return 0;
}

void Engine::update() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) running = false;
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE: running = false; break;
			}
		}
	}
}

void Engine::render() {}

Engine* Engine::instance = 0;
Engine* Engine::getInstance() {
	if (instance == 0) {
		instance = new Engine();
	}
	return instance;
}

Engine::Engine() {}

Engine::~Engine() {
	// core sdl stuff
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
