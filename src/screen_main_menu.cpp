#include "screen_main_menu.h"

MainMenuScreen::MainMenuScreen(SDL_Renderer* _renderer, Config* _config, Data* _data) {
    renderer = _renderer;
    config = _config;
    data = _data;

    text = new TextRenderer(renderer, config, data);

    LOG_F(INFO, "Main menu screen initiliazed.");
}

MainMenuScreen::~MainMenuScreen() {}

void MainMenuScreen::Update(ScreenUpdateEvent event) {}

void MainMenuScreen::Draw(int delta) {}