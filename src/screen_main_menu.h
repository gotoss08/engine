#ifndef MAIN_MENU_SCREEN_H_
#define MAIN_MENU_SCREEN_H_

#include <string>
#include <vector>

#include "libs/loguru.hpp"

#include "config.h"
#include "data.h"
#include "list_widget.h"
#include "screen.h"
#include "text_renderer.h"

class MainMenuScreen : public Screen {
   private:
    TextRenderer* text_renderer;
    ListWidget* list_widget;

   public:
    MainMenuScreen(SDL_Renderer* _renderer, TextRenderer* _text_renderer,
                   Config* _config, Data* _data);
    ~MainMenuScreen();

    void Update(ScreenUpdateEvent event);
    void Draw(int delta);
};

#endif /* MAIN_MENU_SCREEN_H_ */
