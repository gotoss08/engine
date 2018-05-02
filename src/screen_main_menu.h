#ifndef MAIN_MENU_SCREEN_H_
#define MAIN_MENU_SCREEN_H_

#include "libs/loguru.hpp"

#include "screen.h"

class MainMenuScreen : public Screen {
   public:
    void Init();
    void Update(ScreenUpdateEvent event);
    void Draw(SDL_Renderer* renderer, int delta);
};

#endif /* MAIN_MENU_SCREEN_H_ */
