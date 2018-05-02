#ifndef MAIN_MENU_SCREEN_H_
#define MAIN_MENU_SCREEN_H_

#include "libs/loguru.hpp"

#include "config.h"
#include "data.h"
#include "screen.h"
#include "text_renderer.h"

class MainMenuScreen : public Screen {
   private:
    TextRenderer* text;

   public:
    MainMenuScreen(SDL_Renderer* _renderer, Config* _config, Data* _data);
    ~MainMenuScreen();

    void Update(ScreenUpdateEvent event);
    void Draw(int delta);
};

#endif /* MAIN_MENU_SCREEN_H_ */
