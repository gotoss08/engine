#include "screen_main_menu.h"

void Start() { LOG_F(INFO, "START"); }

void Quit() { LOG_F(INFO, "QUIT"); }

MainMenuScreen::MainMenuScreen(SDL_Renderer* _renderer,
                               TextRenderer* _text_renderer, Config* _config,
                               Data* _data) {
    renderer = _renderer;
    config = _config;
    data = _data;

    text_renderer = _text_renderer;

    std::vector<ListWidgetItem> menu_buttons_vector;
    menu_buttons_vector.push_back(ListWidgetItem{"Start", &Start});
    menu_buttons_vector.push_back(ListWidgetItem{"Quit", &Quit});

    list_widget = new ListWidget(text_renderer, 15, 250, menu_buttons_vector);

    LOG_F(INFO, "Main menu screen initiliazed.");
}

MainMenuScreen::~MainMenuScreen() {}

void MainMenuScreen::Update(ScreenUpdateEvent event) {
    for (ScreenKeyEvent key_event : event.keyEvents) {
        switch (key_event.keycode) {
            case SDLK_UP:
                list_widget->Up();
                break;
            case SDLK_DOWN:
                list_widget->Down();
                break;
            case SDLK_RETURN:
                list_widget->Activate();
                break;
        }
    }

    list_widget->Update();
}

void MainMenuScreen::Draw(int delta) {
    // text_renderer->Render("glyph_font", 15, 75, "Main Menu",
    // data->Color("main_menu_text")); text_renderer->Render("glyph_font", 15,
    // 100, "Start", data->Color("main_menu_text_selected"));
    // text_renderer->Render("glyph_font", 15, 125, "Quit",
    // data->Color("main_menu_text"));
    // list_widget->Render();


    std::string long_text = "Alice was beginning to get very tired of sitting by her sister on the bank, and of having nothing to do: once or twice she had peeped into the book her sister was reading, but it had no pictures or conversations in it, 'and what is the use of a book,' thought Alice 'without pictures or conversation?'${newline}${newline}So she was considering in her own mind (as well as she could, for the hot day made her feel very sleepy and stupid), whether the pleasure of making a daisy-chain would be worth the trouble of getting up and picking the daisies, when suddenly a White Rabbit with pink eyes ran close by her.${newline}${newline}There was nothing so VERY remarkable in that; nor did Alice think it so VERY much out of the way to hear the Rabbit say to itself, 'Oh dear! Oh dear! I shall be late!' (when she thought it over afterwards, it occurred to her that she ought to have wondered at this, but at the time it all seemed quite natural); but when the Rabbit actually TOOK A WATCH OUT OF ITS WAISTCOAT- POCKET, and looked at it, and then hurried on, Alice started to her feet, for it flashed across her mind that she had never before seen a rabbit with either a waistcoat-pocket, or a watch to take out of it, and burning with curiosity, she ran across the field after it, and fortunately was just in time to see it pop down a large rabbit-hole under the hedge.";
    text_renderer->SetWordwrap(true);
    // LOG_F(INFO, "main menu screen: wordwrapping: {}", text_renderer->IsWordwrap());
    text_renderer->testRender("glyph_font", 15, 125, long_text, data->Color("main_menu_text"));
}
