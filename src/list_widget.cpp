#include "list_widget.h"

ListWidget::ListWidget(TextRenderer* _text_renderer, int _x, int _y, std::vector<ListWidgetItem> _list_items) {
    text_renderer = _text_renderer;
    x = _x;
    y = _y;
    list_items = _list_items;
}

void ListWidget::Render() {
    int y_cursor = 0;
    for (ListWidgetItem list_item : list_items) {
        RenderedTextMetrics rtm = text_renderer->Render(font_name, x, y + y_cursor, list_item.text, normal_color);
        LOG_F(INFO, "rtm.height={}", rtm.height);
        y_cursor += rtm.height + vertical_spacing;
    }
}