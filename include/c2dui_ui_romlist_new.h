//
// Created by cpasjuste on 14/01/18.
//

#ifndef C2DUI_UI_ROMLIST_NEW_H
#define C2DUI_UI_ROMLIST_NEW_H

#include "cross2d/skeleton/sfml/RectangleShape.hpp"
#include "cross2d/widgets/listbox.h"

class UIRomItem;

class UIRomHighlight;

namespace c2dui {

    class UIRomListNew : public c2dui::UIRomList {

    public:

        UIRomListNew(UIMain *ui, RomList *romList, const c2d::Vector2f &size);

        ~UIRomListNew() override;

        bool onInput(c2d::Input::Player *players) override;

        void onDraw(c2d::Transform &transform, bool draw = true) override;

        void updateRomList() override;

        RomList::Rom *getSelection() override;

    private:

        int rom_index = 0;
        UIRomHighlight *highlight = nullptr;
        std::vector<UIRomItem *> rom_items;
        c2d::Rectangle *rom_items_layer = nullptr;
        c2d::TweenPosition *rom_items_layer_tween = nullptr;
        c2d::Vector2f rom_item_size;
    };
}

#endif //C2DUI_UI_ROMLIST_NEW_H
