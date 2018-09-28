//
// Created by cpasjuste on 23/11/16.
//

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

Skin::Skin(const std::string &p, const std::vector<Button> &btns) {

    this->path = p + "skin/";

    tex_title = new C2DTexture((path + "title.png").c_str());
    tex_title->setFiltering(C2D_TEXTURE_FILTER_POINT);
    font = new C2DFont();
    font->loadFromFile(path + "default.ttf");
    font->setFiltering(C2D_TEXTURE_FILTER_POINT);
    font->setYOffset(-5);

    // load buttons textures
    buttons = btns;
    for (auto &button : buttons) {
        button.path = path + "buttons/" + std::to_string(button.id) + ".png";
    }
}

Skin::Button *Skin::getButton(int id) {

    for (unsigned int i = 0; i < buttons.size(); i++) {
        if (buttons[i].id == id) {
            return &buttons[i];
        }
    }
    return nullptr;
}

Skin::~Skin() {

    if (font) {
        delete (font);
    }

    buttons.clear();
}
