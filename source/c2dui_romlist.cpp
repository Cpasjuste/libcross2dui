//
// Created by cpasjuste on 19/10/16.
//

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

RomList::RomList(UIMain *_ui, const std::string &emuVersion) {

    printf("RomList()\n");

    ui = _ui;
    hardwareList = ui->getConfig()->getHardwareList();
    paths = ui->getConfig()->getRomPaths();

    // UI
    rect = new C2DRectangle(
            Vector2f(ui->getSize().x - 8, ui->getSize().y - 8));
    ui->getSkin()->loadRectangleShape(rect, {"MAIN"});

    texture = ui->getSkin()->tex_title;
    texture->setOrigin(Origin::Center);
    texture->setPosition(Vector2f(rect->getSize().x / 2, rect->getSize().y / 2));
    float scaling = std::min(
            (rect->getSize().x - 64) / texture->getTextureRect().width,
            (rect->getSize().y - 64) / texture->getTextureRect().height);
    texture->setScale(scaling, scaling);
    rect->add(texture);

    strcpy(text_str, "Roms found: 0/0");
    text = new Text(text_str, C2D_DEFAULT_CHAR_SIZE, ui->getSkin()->font);
    text->setOrigin(Origin::BottomLeft);
    text->setOutlineColor(Color::Black);
    text->setOutlineThickness(2);
    text->setPosition(8, rect->getSize().y - 16);
    rect->add(text);

    auto *version = new Text(emuVersion, C2D_DEFAULT_CHAR_SIZE, ui->getSkin()->font);
    version->setOrigin(Origin::BottomRight);
    version->setOutlineColor(Color::Black);
    version->setOutlineThickness(2);
    version->setPosition(rect->getSize().x - 16, rect->getSize().y - 16);
    rect->add(version);

    ui->add(rect);
    ui->flip();
    // UI

    printf("RomList: building list...\n");
    time_start = ui->getElapsedTime().asSeconds();

    for (auto &path : *paths) {
        if (!path.empty()) {
            printf("RomList: scanning path: `%s`\n", path.c_str());
            std::vector<Io::File> filesList = ui->getIo()->getDirList(path);
            files.emplace_back(filesList);
            printf("RomList: found %i files in `%s`\n", (int) filesList.size(), path.c_str());
        }
    }
    printf("RomList()\n");
}

void RomList::build() {

    printf("RomList::build()\n");

    // build favorites
    std::string favPath = *ui->getConfig()->getHomePath() + "favorites.bin";
    std::ifstream favFile(favPath);
    if (favFile.is_open()) {
        std::string line;
        while (std::getline(favFile, line)) {
            auto rom = std::find_if(list.begin(), list.end(), [line](Rom *rom) -> bool {
                return line == rom->path;
            });
            if (rom != list.end()) {
                (*rom)->hardware |= HARDWARE_PREFIX_FAV;
            }
        }
        favFile.close();
    }

    float time_spent = ui->getElapsedTime().asSeconds() - time_start;
    printf("RomList::build(): list built in %f\n", time_spent);

    // UI
    // reset title texture for later use
    texture->setOrigin(Origin::TopLeft);
    texture->setPosition(0, 0);
    texture->setScale(1, 1);
    rect->remove(texture);
    // remove ui widgets
    delete (rect);
}

void RomList::addFav(Rom *rom) {

    if (!rom || rom->hardware & HARDWARE_PREFIX_FAV) {
        printf("RomList::addFav: already in favorites\n");
        return;
    }

    rom->hardware |= HARDWARE_PREFIX_FAV;

    std::string favPath = *ui->getConfig()->getHomePath() + "favorites.bin";
    std::ofstream favFile(favPath, std::ios::app);
    if (favFile.is_open()) {
        favFile << rom->path;
        favFile << "\n";
        favFile.close();
    }
}

void RomList::removeFav(Rom *rom) {

    if (!rom || !(rom->hardware & HARDWARE_PREFIX_FAV)) {
        printf("RomList::addFav: not in favorites\n");
        return;
    }

    rom->hardware &= ~HARDWARE_PREFIX_FAV;

    // TODO: only remove specific line ?
    std::string favPath = *ui->getConfig()->getHomePath() + "favorites.bin";
    std::ofstream favFile(favPath, std::ios::trunc);
    if (favFile.is_open()) {
        for (auto &r : list) {
            if (r->flags & HARDWARE_PREFIX_FAV) {
                favFile << rom->path;
                favFile << "\n";
            }
        }
        favFile.close();
    }
}

RomList::~RomList() {

    printf("~RomList()\n");

    for (auto &rom : list) {
        if (rom && !rom->parent && rom->icon) {
            delete (rom->icon);
            rom->icon = nullptr;
        }
        if (rom) {
            delete (rom);
        }
    }
}
