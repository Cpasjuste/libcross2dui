//
// Created by cpasjuste on 14/01/18.
//

#ifndef GUI_ROMIST_H
#define GUI_ROMIST_H

#include "cross2d/skeleton/sfml/RectangleShape.hpp"
#include "cross2d/widgets/listbox.h"

class UIRomInfo;

namespace c2dui {

    class UIRomList : public c2d::RectangleShape {

    public:

        UIRomList(UIMain *ui, RomList *romList, const c2d::Vector2f &size);

        ~UIRomList();

        bool onInput(c2d::Input::Player *players) override;

        void onDraw(c2d::Transform &transform) override;

        void updateRomList();

        RomList *getRomList();

        RomList::Rom *getSelection();

    private:

        UIMain *ui = nullptr;
        UIRomInfo *rom_info = nullptr;
        c2d::ListBox *list_box = nullptr;

        RomList *rom_list = nullptr;
        std::vector<RomList::Rom *> roms;
        int rom_index = 0;

        bool show_preview = false;
        int title_loaded = 0;
        int load_delay = 100;
        c2d::C2DClock timer_load;
    };
}

#endif //GUI_ROMIST_H
