//
// Created by cpasjuste on 29/05/18.
//

#ifndef PFBA_ROMLIST_H
#define PFBA_ROMLIST_H

#include <string>

#include "c2dui_gui_main.h"
#include "c2dui_romlist.h"

#include "burn.h"

#define HARDWARE_PREFIX_ALL 0xffffffff

class RomList : public c2dui::C2DUIRomList {

public:

    RomList(c2dui::C2DUIGuiMain *ui, const std::string &emuVersion);

    void build() override;

    bool isHardware(int hardware, int type) override {
        return (((hardware | HARDWARE_PREFIX_CARTRIDGE) ^ HARDWARE_PREFIX_CARTRIDGE)
                & 0xff000000) == (unsigned int) type;
    }
};

#endif //PFBA_ROMLIST_H
