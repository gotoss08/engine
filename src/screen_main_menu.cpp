#include "screen_main_menu.h"

MainMenuScreen::MainMenuScreen(SDL_Renderer* _renderer, TextRenderer* _text_renderer, Config* _config, Data* _data) {
    renderer = _renderer;
    config = _config;
    data = _data;

    text = _text_renderer;

    LOG_F(INFO, "Main menu screen initiliazed.");
}

MainMenuScreen::~MainMenuScreen() {}

void MainMenuScreen::Update(ScreenUpdateEvent event) {}

void MainMenuScreen::Draw(int delta) {
    LOG_F(INFO, "main menu: rendering");
    text->Render("glyph_font", 15, 75, "Main Menu", data->Color("main_menu_text"));
    text->Render("glyph_font", 15, 100, "Start", data->Color("main_menu_text_selected"));
    text->Render("glyph_font", 15, 125, "Quit", data->Color("main_menu_text"));
}