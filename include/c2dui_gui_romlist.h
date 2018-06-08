//
// Created by cpasjuste on 14/01/18.
//

#ifndef GUI_ROMIST_H
#define GUI_ROMIST_H

class C2DUIGuiRomInfo;

namespace c2dui {

    class C2DUIGuiRomList : public c2d::Rectangle {

    public:

        C2DUIGuiRomList(C2DUIGuiMain *ui, C2DUIRomList *romList, const c2d::Vector2f &size);

        ~C2DUIGuiRomList();

        int update();

        void updateRomList();

        C2DUIRomList *getRomList();

        C2DUIRomList::Rom *getSelection();

        void setLoadDelay(int delay);

    private:

        C2DUIGuiMain *ui = NULL;
        C2DUIGuiRomInfo *rom_info = NULL;
        c2d::ListBox *list_box = NULL;

        C2DUIRomList *rom_list = NULL;
        std::vector<C2DUIRomList::Rom *> roms;
        int rom_index = 0;

        int title_loaded = 0;
        int load_delay = 100;
        c2d::C2DClock timer_load;
    };
}

#endif //GUI_ROMIST_H
