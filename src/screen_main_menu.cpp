#include "screen_main_menu.h"

void Start() {
    LOG_F(INFO, "START");
}

void Quit() {
    LOG_F(INFO, "QUIT");
}

MainMenuScreen::MainMenuScreen(SDL_Renderer* _renderer, TextRenderer* _text_renderer, Config* _config, Data* _data) {
    renderer = _renderer;
    config = _config;
    data = _data;

    text_renderer = _text_renderer;

    std::vector<ListWidgetItem> menu_buttons_vector;
    menu_buttons_vector.push_back(ListWidgetItem{"Start", &Start});
    menu_buttons_vector.push_back(ListWidgetItem{"Quit", &Quit});

    list_widget = new ListWidget(text_renderer, 15, 75, menu_buttons_vector);

    LOG_F(INFO, "Main menu screen initiliazed.");
}

MainMenuScreen::~MainMenuScreen() {}

void MainMenuScreen::Update(ScreenUpdateEvent event) {}

void MainMenuScreen::Draw(int delta) {
    // text_renderer->Render("glyph_font", 15, 75, "Main Menu", data->Color("main_menu_text"));
    // text_renderer->Render("glyph_font", 15, 100, "Start", data->Color("main_menu_text_selected"));
    // text_renderer->Render("glyph_font", 15, 125, "Quit", data->Color("main_menu_text"));
    list_widget->Render();
}