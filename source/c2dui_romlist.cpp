//
// Created by cpasjuste on 19/10/16.
//

#include <vector>
#include <string>
#include <algorithm>

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

C2DUIRomList::C2DUIRomList(C2DUIGuiMain *_ui, const std::string &emuVersion) {

    printf("C2DUIRomList()\n");

    ui = _ui;
    hardwareList = ui->getConfig()->getHardwareList();
    paths = ui->getConfig()->getRomPaths();

    // UI
    rect = new C2DRectangle(
            Vector2f(ui->getRenderer()->getSize().x - 8, ui->getRenderer()->getSize().y - 8));
    rect->setPosition(4, 4);
    rect->setFillColor(Color::Gray);
    rect->setOutlineColor(Color::Orange);
    rect->setOutlineThickness(4);

    texture = ui->getSkin()->tex_title;
    texture->setOriginCenter();
    texture->setPosition(Vector2f(rect->getSize().x / 2, rect->getSize().y / 2));
    float scaling = std::min(
            (rect->getSize().x - 64) / texture->getSize().x,
            (rect->getSize().y - 64) / texture->getSize().y);
    texture->setScale(scaling, scaling);
    rect->add(texture);

    strcpy(text_str, "Roms found: 0/0");
    text = new Text(text_str, *ui->getSkin()->font);
    text->setOriginBottomLeft();
    text->setOutlineColor(Color::Black);
    text->setOutlineThickness(2);
    text->setPosition(16, rect->getSize().y - 8);
    rect->add(text);

    auto *version = new Text(emuVersion, *ui->getSkin()->font);
    version->setOriginBottomRight();
    version->setOutlineColor(Color::Black);
    version->setOutlineThickness(2);
    version->setPosition(rect->getSize().x - 8, rect->getSize().y - 8);
    rect->add(version);

    ui->getRenderer()->add(rect);
    // UI

    printf("C2DUIRomList: building list...\n");
    time_start = ui->getRenderer()->getElapsedTime().asSeconds();

    for (unsigned int i = 0; i < paths->size(); i++) {
        if (!paths[i].empty()) {
            files[i] = ui->getIo()->getDirList(paths->at(i).c_str());
            //printf("RomList: found %i files in `%s`\n", (int) files[i].size(), paths[i].c_str());
        }
    }
}

void C2DUIRomList::build() {

    printf("C2DUIRomList::build()\n");

    float time_spent = ui->getRenderer()->getElapsedTime().asSeconds() - time_start;
    printf("C2DUIRomList::build(): list built in %f\n", time_spent);

    files->clear();

    // UI
    // reset title texture for later use
    texture->setOriginTopLeft();
    texture->setPosition(0, 0);
    texture->setScale(1, 1);
    rect->removeChild(texture);
    // remove ui widgets
    delete (rect);
}

C2DUIRomList::~C2DUIRomList() {

    printf("~C2DUIRomList()\n");

    for (auto &rom : list) {
        delete (rom);
    }

    list.clear();
}
