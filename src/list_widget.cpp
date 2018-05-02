#include "list_widget.h"

ListWidget::ListWidget(TextRenderer* _text_renderer, int _x, int _y, std::vector<ListWidgetItem> _list_items) {
    text_renderer = _text_renderer;
    x = _x;
    y = _y;
    list_items = _list_items;
}

void ListWidget::Render() {
    int y_cursor = 0;
    int render_item_index = 0;
    SDL_Color current_color;
    for (ListWidgetItem list_item : list_items) {
        if (render_item_index == item_index) current_color = highlighted_color;
        else current_color = normal_color;

        RenderedTextMetrics rtm = text_renderer->Render(font_name, x, y + y_cursor, list_item.text, current_color);
        y_cursor += rtm.height + vertical_spacing;

        render_item_index++;
    }
}

void ListWidget::Update() {
    if (item_index < 0) item_index = list_items.size() - 1;
    if (item_index == list_items.size()) item_index = 0;
}

void ListWidget::Up() { item_index--; }

void ListWidget::Down() { item_index++; }

void ListWidget::Activate() { list_items[item_index].action(); }
