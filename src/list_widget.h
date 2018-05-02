#ifndef LIST_WIDGET_H_
#define LIST_WIDGET_H_

#include <string>
#include <vector>

#include "libs/loguru.hpp"

#include "text_renderer.h"

struct ListWidgetItem {
    std::string text;
    void(*action)();
};

class ListWidget {
   private:
    TextRenderer* text_renderer;

    int x;
    int y;

    std::vector<ListWidgetItem> list_items;

    std::string font_name = "main_menu_font";
    SDL_Color normal_color{164, 167, 176, 255};
    SDL_Color highlighted_color{214, 216, 221, 255};

    int vertical_spacing = 7;

    int item_index = 0;
   public:
    ListWidget(TextRenderer* _text_renderer, int _x, int _y, std::vector<ListWidgetItem> _list_items);
    void Render();
    void Update();
    void Up();
    void Down();
    void Activate();
};

#endif /* LIST_WIDGET_H_ */