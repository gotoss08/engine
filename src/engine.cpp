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
        LOG_F(ERROR, "Unable to initialize SDL: {}", SDL_GetError());
        successfull_load = false;
    }

    /* sdl2_ttf lib initialization */
    if (TTF_Init() != 0) {
        LOG_F(ERROR, "Unable to initialize SDL_TTF: {}", SDL_GetError());
        successfull_load = false;
    }

    data = new Data();
    if (data->Load(config->Get("data-file")) != 0) {
        LOG_F(ERROR, "Unable to load data");
        successfull_load = false;
    }

    /* window creating */
    Uint32 window_resizable_flag = config->Get("resizable") == "true" ? SDL_WINDOW_RESIZABLE : 0;
    Uint32 window_fullscreen_flag = config->Get("display-mode") == "fullscreen" ? SDL_WINDOW_FULLSCREEN : 0;
    Uint32 window_borderless_flag = config->Get("display-mode") == "borderless" ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                              SDL_WINDOW_OPENGL | window_resizable_flag | window_fullscreen_flag | window_borderless_flag);
    if (window == NULL) {
        LOG_F(ERROR, "Unable to create window: %s\n", SDL_GetError());
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
    if (renderer == NULL) {
        LOG_F(ERROR, "Unable to create renderer: {}", SDL_GetError());
        successfull_load = false;
    }

    /* character maps generation */
    text = new TextRenderer(renderer, config, data);
    text->SetWordwrap(true);
    if (text->GenerateCharacterMap()) {
        LOG_F(ERROR, "Unable to generate characters map.");
        successfull_load = false;
    }

    // set start screen to main menu screen
    SetScreen(new MainMenuScreen());

    /* if no errors proceed */
    if (successfull_load)
        return 0;
    else
        return -1;
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
            ticks++;

            ScreenUpdateEvent screenEvent;
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_WINDOWEVENT) {
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            config->Set("window-width", std::to_string(event.window.data1));
                            config->Set("window-height", std::to_string(event.window.data2));
                            break;
                    }
                } else if (event.type == SDL_QUIT) {
                    running = false;
                } else if (event.type == SDL_KEYDOWN) {
                    screenEvent.keyEvents.push_back(ScreenKeyEvent{event.key.state, event.key.repeat, event.key.keysym.sym});

                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                    }
                } else if (event.type == SDL_MOUSEMOTION) {
                    screenEvent.moveEvents.push_back(ScreenMouseMoveEvent{event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel});
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    screenEvent.clickEvents.push_back(
                        ScreenMouseClickEvent{event.button.button, event.button.state, event.button.clicks, event.button.x, event.button.y});
                } else if (event.type == SDL_MOUSEWHEEL) {
                    screenEvent.scroll = event.wheel.y;
                }
            }

            screen->Update(screenEvent);
        }

        SDL_Color clear_color = data->Color("clear_color");

        SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        SDL_RenderClear(renderer);

        // render here
        screen->Draw(renderer, delta_time.count());

        frames++;

        auto frame_render_time = std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - time_start);

        if (std::chrono::duration_cast<std::chrono::seconds>(clock::now() - timer).count() >= 1) {
            LOG_F(INFO, "frames: {}, ticks: {}", frames, ticks);
            fps_text = fmt::format(
                "${{color_name=fps_color_value}}{} ${{color_name=fps_color_text}}fps | ${{color_name=fps_color_value}}{} "
                "${{color_name=fps_color_text}}ms frame render time",
                frames, std::chrono::duration_cast<std::chrono::milliseconds>(frame_render_time).count());

            timer = clock::now();
            ticks = 0;
            frames = 0;
        }

        text->Render("debug_font", 7, 7, fps_text, data->Color("fps_color_text"));

        SDL_RenderPresent(renderer);

        if (frame_timestep > frame_render_time) {
            SDL_Delay(std::chrono::duration_cast<std::chrono::milliseconds>(frame_timestep - frame_render_time).count());
        }
    }
    return 0;
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
