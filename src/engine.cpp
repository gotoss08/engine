/*
 * engine.cpp
 *
 *  Created on: Apr 16, 2018
 *      Author: gotoss08
 */

#include "engine.h"

int Engine::Init(std::string title)
{
	config = new Config();
	config->Load();

	Engine::width = is_number(config->Get("window-width")) ? std::stoi(config->Get("window-width").c_str()) : 640;
	Engine::height = is_number(config->Get("window-height")) ? std::stoi(config->Get("window-height").c_str()) : 480;

	/* sdl2 lib initialization */
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
		successfull_load = false;
	}

	/* sdl2_ttf lib initialization */
	if (TTF_Init() != 0)
	{
		SDL_Log("Unable to initialize SDL_TTF: %s\n", TTF_GetError());
		successfull_load = false;
	}

	data = new Data();
	if (data->Load(config->Get("data-file")) != 0)
	{
		SDL_Log("Unable to load data");
		successfull_load = false;
	}

	/* window creating */
	Uint32 window_resizable_flag = config->Get("resizable") == "true" ? SDL_WINDOW_RESIZABLE : 0;
	Uint32 window_fullscreen_flag = config->Get("display-mode") == "fullscreen" ? SDL_WINDOW_FULLSCREEN : 0;
	Uint32 window_borderless_flag = config->Get("display-mode") == "borderless" ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | window_resizable_flag | window_fullscreen_flag | window_borderless_flag);
	if (window == NULL)
	{
		SDL_Log("Unable to create window: %s\n", SDL_GetError());
		successfull_load = false;
	}

	/* renderer creation */
	Uint32 renderer_flags;

	if (config->Get("renderer-acceleration") == "software")
		renderer_flags = SDL_RENDERER_SOFTWARE;
	else if (config->Get("vsync") == "on")
		renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	else
		renderer_flags = SDL_RENDERER_ACCELERATED;

	renderer = SDL_CreateRenderer(window, -1, renderer_flags);
	if (renderer == NULL)
	{
		SDL_Log("Unable to create renderer: %s\n", SDL_GetError());
		successfull_load = false;
	}

	/* character maps generation */
	text = new TextRenderer(renderer, config, data);
	text->SetWordwrap(true);
	if (text->GenerateCharacterMap())
	{
		SDL_Log("Unable to generate characters map.");
		successfull_load = false;
	}

	/* if no errors proceed */
	if (successfull_load)
		return 0;
	else
		return -1;
}

int Engine::Loop(int _fps, int _ups)
{
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

	while (running)
	{
		// calcualte awaited timesteps
		std::chrono::nanoseconds frame_timestep(1000000000 / fps);
		std::chrono::nanoseconds tick_timestep(1000000000 / ups);

		auto delta_time = clock::now() - time_start;
		time_start = clock::now();
		lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);

		while (lag >= tick_timestep)
		{
			lag -= tick_timestep;
			update();
			ticks++;
		}

		SDL_Color clear_color = data->Color("clear_color");

		SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		SDL_RenderClear(renderer);

		render();

		frames++;

		auto frame_render_time = std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - time_start);

		if (std::chrono::duration_cast<std::chrono::seconds>(clock::now() - timer).count() >= 1)
		{
			SDL_Log("frames: %d, ticks: %d | per second\n", frames, ticks);
			fps_text = "${color_name=green}" + std::to_string(frames) + "${color_name=white}${font=debug_font2}fps | ${color_name=green}${font=default}" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(frame_render_time).count()) + "${color=255,255,255,255}ms ${font=debug_font3}frame${font=default} render${newline} time";

			timer = clock::now();
			ticks = 0;
			frames = 0;
		}

		SDL_Color fps_color = {236, 208, 120, 255};

		// text->Render("debug_font", 7, 7, fps_text, fps_color);

		// text->Render("debug_font", 7, 50, "test string that ${newline${tag inside${tag inside inside tag} tag}}${another tag}contains tags", fps_color);

		/*std::string long_text = "${color_name=text}${color_name=alice}Alice${color_name=text} was beginning to get very tired of sitting by her sister on${font_name=debug_font3} the ${font_name=debug_font}bank, and of having nothing to do: once or twice${color_name=the} she ${color_name=text}had peeped into${font_name=debug_font3} the ${font_name=debug_font}book her sister was reading, ${font_name=debug_font2}but${font_name=debug_font} it had no pictures or conversations in it, 'and what is${font_name=debug_font3} the ${font_name=debug_font}use of a book,' thought ${color_name=alice}Alice${color_name=text} 'without pictures or conversations?'${newline}${newline}So${color_name=the} she ${color_name=text}was considering in her own mind (as well as${color_name=the} she ${color_name=text}could, for${font_name=debug_font3} the ${font_name=debug_font}hot day made her feel very sleepy and stupid), whether${font_name=debug_font3} the ${font_name=debug_font}pleasure of making a daisy-chain would be worth${font_name=debug_font3} the ${font_name=debug_font}trouble of getting up and picking${font_name=debug_font3} the ${font_name=debug_font}daisies, when suddenly a White ${color_name=rabbit}Rabbit${color_name=text} with pink eyes ran close by her.${newline}${newline}There was nothing so very remarkable in that; nor did ${color_name=alice}Alice${color_name=text} think it so very much out of${font_name=debug_font3} the ${font_name=debug_font}way to hear${font_name=debug_font3} the ${font_name=debug_font}${color_name=rabbit}Rabbit${color_name=text} say to itself, 'Oh dear! Oh dear! I shall be late!' (when${color_name=the} she ${color_name=text}thought it over afterwards, it occurred to her that${color_name=the} she ${color_name=text}ought to have wondered at this, ${font_name=debug_font2}but${font_name=debug_font} at${font_name=debug_font3} the ${font_name=debug_font}time it all seemed quite natural); ${font_name=debug_font2}but${font_name=debug_font} when${font_name=debug_font3} the ${font_name=debug_font}${color_name=rabbit}Rabbit${color_name=text} actually took a watch out of its waistcoat-pocket, and looked at it, and then hurried on, ${color_name=alice}Alice${color_name=text} started to her feet, for it flashed across her mind that${color_name=the} she ${color_name=text}had never before seen a ${color_name=rabbit}rabbit${color_name=text} with either a waistcoat-pocket, or a watch to take out of it, and burning with curiosity,${color_name=the} she ${color_name=text}ran across${font_name=debug_font3} the ${font_name=debug_font}field after it, and fortunately was just in time to see it pop down a large ${color_name=rabbit}rabbit${color_name=text}-hole under${font_name=debug_font3} the ${font_name=debug_font}hedge.";*/
		// text->SetPadding(10, 10, 10, 10);
		// text->Render("debug_font", 0, 0, long_text, fps_color);

		std::string roguelike_message_log = "${color_name=roguelike-text}The ${color_name=roguelike-attacker}giant centipede${color_name=roguelike-text} hits ${color_name=roguelike-attacked}you${color_name=roguelike-text}, dealing ${color_name=roguelike-damage}11${color_name=roguelike-text} damage";
		text->SetPadding(10, 10, 10, 10);
		text->Render("roguelike_font", 480, 180, roguelike_message_log, fps_color);

		SDL_RenderPresent(renderer);

		if (frame_timestep > frame_render_time)
		{
			SDL_Delay(std::chrono::duration_cast<std::chrono::milliseconds>(frame_timestep - frame_render_time).count());
		}
	}
	return 0;
}

void Engine::update()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_WINDOWEVENT)
		{
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				SDL_Log("window resized: %dx%d\n", event.window.data1, event.window.data2);
				config->Set("window-width", std::to_string(event.window.data1));
				config->Set("window-height", std::to_string(event.window.data2));
				break;
			}
		}
		if (event.type == SDL_QUIT)
			running = false;
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				running = false;
				break;
			}
		}
	}
}

void Engine::render() {}

Engine *Engine::instance = 0;
Engine *Engine::getInstance()
{
	if (instance == 0)
	{
		instance = new Engine();
	}
	return instance;
}

Engine::Engine() {}

Engine::~Engine()
{
	delete config;
	delete data;
	delete text;

	// core sdl stuff
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
