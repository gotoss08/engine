/*
 * engine.cpp
 *
 *  Created on: Apr 16, 2018
 *      Author: gotoss08
 */

#include "engine.h"

int Engine::Init(std::string title) {
	config = new Config();
	config->Load();

	Engine::width = is_number(config->Get("window-width")) ? std::stoi(config->Get("window-width").c_str()) : 640;
	Engine::height = is_number(config->Get("window-height")) ? std::stoi(config->Get("window-height").c_str()) : 480;

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
	if (data->Load(config->Get("data-file")) != 0) {
		SDL_Log("Unable to load data");
		successfull_load = false;
	}

	/* window creating */
	Uint32 window_resizable_flag = config->Get("resizable") == "true" ? SDL_WINDOW_RESIZABLE : 0;
	Uint32 window_fullscreen_flag = config->Get("display-mode") == "fullscreen" ? SDL_WINDOW_FULLSCREEN : 0;
	Uint32 window_borderless_flag = config->Get("display-mode") == "borderless" ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | window_resizable_flag | window_fullscreen_flag | window_borderless_flag);
	if (window == NULL) {
		SDL_Log("Unable to create window: %s\n", SDL_GetError());
		successfull_load = false;
	}

	/* renderer creation */
	Uint32 renderer_flags;

	if (config->Get("renderer-acceleration") == "software") renderer_flags = SDL_RENDERER_SOFTWARE;
	else if (config->Get("vsync") == "on") renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	else renderer_flags = SDL_RENDERER_ACCELERATED;

	renderer = SDL_CreateRenderer(window, -1, renderer_flags);
	if (renderer == NULL) {
		SDL_Log("Unable to create renderer: %s\n", SDL_GetError());
		successfull_load = false;
	}

	/* character maps generation */
	text = new TextRenderer(config, data);
	std::cout << "alignement: " << text->GetAlignment() << std::endl;
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

	using clock = std::chrono::high_resolution_clock;
	std::chrono::nanoseconds lag(0);
	auto time_start = clock::now();
	auto timer = clock::now();

	int ticks = 0;
	int frames = 0;

	Timer engine_text_rendering_timer;
	Timer sdl_text_rendering_timer;

	std::string fps_text = "";

	while (running) {
		// calcualte awaited timesteps
		std::chrono::nanoseconds frame_timestep(1000000000 / fps);
		std::chrono::nanoseconds tick_timestep(1000000000 / ups);

		auto delta_time = clock::now() - time_start;
		time_start = clock::now();
		lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);

		while (lag >= tick_timestep) {
			lag -= tick_timestep;
			update();
			ticks++;
		}

		SDL_SetRenderDrawColor(renderer, 83, 119, 122, 255);
		SDL_RenderClear(renderer);

		render();

		frames++;

		auto frame_render_time = std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - time_start);

		if (std::chrono::duration_cast<std::chrono::seconds>(clock::now() - timer).count() >= 1) {
			SDL_Log("frames: %d, ticks: %d | per second\n", frames, ticks);
			fps_text = "{{color_name=green}}" + std::to_string(frames) + "{{color_name=white}}{{font=debug_font2}}fps | {{color_name=green}}{{font=default}}" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(frame_render_time).count()) + "{{color=255,255,255,255}}ms {{font=debug_font3}}frame{{font=default}} render{{newline}} time";

			timer = clock::now();
			ticks = 0;
			frames = 0;
		}

		SDL_Color fps_color = {236,208,120,255};
		text->Render(renderer, "debug_font", 7, 7, fps_text, fps_color);

		std::string long_text = "Alice was beginning to get very tired of sitting by her sister on the bank, and of having nothing to do: once or twice she had peeped into the book her sister was reading, but it had no pictures or conversations in it, ‘and what is the use of a book,’ thought Alice ‘without pictures or conversations?’";
		text->Render(renderer, "debug_font", 7, 50, long_text, fps_color);

		SDL_RenderPresent(renderer);

		if (frame_timestep > frame_render_time) {
			SDL_Delay(std::chrono::duration_cast<std::chrono::milliseconds>(frame_timestep - frame_render_time).count());
		}
	}
	return 0;
}

void Engine::update() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_WINDOWEVENT) {
			switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					SDL_Log("window resized: %dx%d\n", event.window.data1, event.window.data2);
					config->Set("window-width", std::to_string(event.window.data1));
					config->Set("window-height", std::to_string(event.window.data2));
					break;
			}
		}
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
	delete config;
	delete data;
	delete text;

	// core sdl stuff
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
