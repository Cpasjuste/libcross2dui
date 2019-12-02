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
using namespace ss_api;

UIRomList::UIRomList(UIMain *u, RomList *romList, const c2d::Vector2f &size) : RectangleShape(size) {

    printf("UIRomList\n");

    ui = u;
    rom_list = romList;
}

Game UIRomList::getSelection() {
    return Game();
}

RomList *UIRomList::getRomList() {
    return rom_list;
}

Texture *UIRomList::getPreviewTexture(const ss_api::Game &game, bool isPreview) {

    // load image
    // TODO: verify loading with psnes and no db.xml)
    // TODO: fix game.path
    C2DTexture *texture = nullptr;
    std::string path = isPreview ?
                       ui->getConfig()->getRomPath(0) + game.getMedia(Game::Media::Type::Box3D).url
                                 : ui->getConfig()->getRomPath(0) + game.getMedia(Game::Media::Type::SS).url;

    printf("getPreviewTexture(%s, %i)\n", path.c_str(), isPreview);
#ifndef __SWITCH__
    // TODO: fix switch stat/fopen slow on non existing files
    // TODO: fix sscrap parent (cloneof)
    /*
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
    */
#endif
    if (ui->getIo()->exist(path)) {
        texture = new C2DTexture(path);
    }

    return texture;
}

void UIRomList::filterRomList() {

    games = ss_api::Api::gameListFilter(rom_list->gameList.games,
                                        ui->getConfig()->get(Option::Id::GUI_SHOW_ALL)->getValueString() == "AVAILABLE",
                                        ui->getConfig()->get(Option::Id::GUI_FILTER_CLONES)->getValueBool(),
                                        ui->getConfig()->get(Option::Id::GUI_FILTER_SYSTEM)->getValueString(),
                                        ui->getConfig()->get(Option::Id::GUI_FILTER_EDITOR)->getValueString(),
                                        ui->getConfig()->get(Option::Id::GUI_FILTER_DEVELOPER)->getValueString(),

                                        ui->getConfig()->get(Option::Id::GUI_FILTER_PLAYERS)->getValueString(),
                                        ui->getConfig()->get(Option::Id::GUI_FILTER_RATING)->getValueString(),
                                        "All",
                                        ui->getConfig()->get(Option::Id::GUI_FILTER_ROTATION)->getValueString(),
                                        ui->getConfig()->get(Option::Id::GUI_FILTER_RESOLUTION)->getValueString(),
                                        ui->getConfig()->get(Option::Id::GUI_FILTER_DATE)->getValueString(),
                                        ui->getConfig()->get(Option::Id::GUI_FILTER_GENRE)->getValueString()
    );
}

void UIRomList::updateRomList() {
}

UIRomList::~UIRomList() {
    printf("~UIRomList\n");
    delete (rom_list);
}
