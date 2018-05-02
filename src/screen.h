#ifndef SCREEN_H_
#define SCREEN_H_

#include <vector>

#include <SDL2/SDL.h>

struct ScreenKeyEvent {
    Uint8 state;
    Uint8 repeat;
    SDL_Keycode keycode;
};

struct ScreenMouseMoveEvent {
    Sint32 x;
    Sint32 y;
    Sint32 xrel;
    Sint32 yrel;
};

struct ScreenMouseClickEvent {
    Uint8 button;
    Uint8 state;
    Uint8 clicks;
    Sint32 x;
    Sint32 y;
};

struct ScreenUpdateEvent {
    std::vector<ScreenKeyEvent> keyEvents;
    std::vector<ScreenMouseMoveEvent> moveEvents;
    std::vector<ScreenMouseClickEvent> clickEvents;
    Sint32 scroll;
};

class Screen {
   public:
    Screen() {}
    virtual ~Screen() {}
    virtual void Init() = 0;
    virtual void Update(ScreenUpdateEvent event) = 0;
    virtual void Draw(SDL_Renderer* renderer, int delta) = 0;
};

#endif /* SCREEN_H_ */
