//
// Created by cpasjuste on 22/11/16.
//
#include <algorithm>

#include "c2dui.h"

/// pFBA
#define BDF_ORIENTATION_FLIPPED     (1 << 1)
#define BDF_ORIENTATION_VERTICAL    (1 << 2)

using namespace c2d;
using namespace c2dui;

UIRomList::UIRomList(UIMain *u, RomList *romList, const c2d::Vector2f &size) : RectangleShape(size) {

    printf("UIRomList\n");

    ui = u;
    rom_list = romList;
}

RomList::Rom *UIRomList::getSelection() {
    return nullptr;
}

RomList *UIRomList::getRomList() {
    return rom_list;
}

Texture *UIRomList::getPreviewTexture(RomList::Rom *rom, bool isPreview) {

    // load image
    // TODO: verify loading with psnes and no db.xml)
    C2DTexture *texture = nullptr;
    std::string name = Utility::removeExt(rom->drv_name);
    std::string type = isPreview ? "previews" : "titles";
    std::string home_path = ui->getConfig()->getHomePath();
    std::string path;

    path = home_path + type + "/" + name + ".png";
    printf("getPreviewTexture(%s, %i)\n", path.c_str(), isPreview);
#ifndef __SWITCH__
    // TODO: fix switch stat/fopen slow on non existing files
    if (!ui->getIo()->exist(path)) {
        path = home_path + type + "/" + name + ".jpg";
        printf("getPreviewTexture(%s, %i)\n", path.c_str(), isPreview);
        if (!ui->getIo()->exist(path) && rom->parent) {
            name = Utility::removeExt(rom->parent);
            path = home_path + type + "/" + name + ".png";
            printf("getPreviewTexture(%s, %i)\n", path.c_str(), isPreview);
            if (!ui->getIo()->exist(path)) {
                path = home_path + type + "/" + name + ".jpg";
                printf("getPreviewTexture(%s, %i)\n", path.c_str(), isPreview);
            }
        }
    }
#endif
    if (ui->getIo()->exist(path)) {
        texture = new C2DTexture(path);
    }

    return texture;
}

void UIRomList::filterRomList() {

    roms.clear();

    static RomList *list = rom_list;
    bool showClone = ui->getConfig()->get(Option::Id::GUI_SHOW_CLONES)->getValueBool();
    Option *hwOpt = ui->getConfig()->get(Option::Id::GUI_SHOW_HARDWARE);
    int showHardwareCfg = hwOpt ? hwOpt->getIndex() : 0;
    int showHardware = ui->getConfig()->getHardwareList()->at((unsigned int) showHardwareCfg).prefix;
    printf("showClone: %i, showHardwareCfg: %i, showHardware: %i\n", showClone, showHardwareCfg, showHardware);

    // psnes and pnes have only 2 (0/1) values, so work with value string
    Option *opt = ui->getConfig()->get(Option::Id::GUI_SHOW_ALL);
    int showAll = opt->getValueString() != "ALL";
    if (opt->getValueString() == "FAVORITES") {
        showAll = 2;
    }

    if (showAll == 2) {
        // filter/show favorites
        remove_copy_if(rom_list->list.begin(), rom_list->list.end(), back_inserter(roms),
                       [](RomList::Rom *r) {
                           return !(r->hardware & HARDWARE_PREFIX_FAV);
                       });
    } else {
        remove_copy_if(rom_list->list.begin(), rom_list->list.end(), back_inserter(roms),
                       [showAll, showClone, showHardware](RomList::Rom *r) {
                           return (showAll && r->state != RomList::RomState::WORKING)
                                  || (!showClone && r->parent != nullptr)
                                  || ((unsigned int) showHardware != HARDWARE_PREFIX_ALL
                                      && !list->isHardware(r->hardware, showHardware));
                       });
    }
}

void UIRomList::updateRomList() {
}

UIRomList::~UIRomList() {
    printf("~UIRomList\n");
    delete (rom_list);
}

