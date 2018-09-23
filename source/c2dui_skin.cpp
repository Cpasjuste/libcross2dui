//
// Created by cpasjuste on 23/11/16.
//

#include "c2dui.h"

#ifdef __SWITCH__
#ifdef __PFBA__
#include "../../pfba/data/skin/title.h"
#include "../../pfba/data/skin/default.h"
#elif __PSNES__
#include "../../psnes/data/skin/title.h"
#include "../../psnes/data/skin/default.h"
#elif __PNES__
#include "../../pnes/data/skin/title.h"
#include "../../pnes/data/skin/default.h"
#endif
#endif

using namespace c2d;
using namespace c2dui;

C2DUISkin::C2DUISkin(const std::string &p, const std::vector<Button> &btns) {

    this->path = p + "skin/";

#ifdef __SWITCH__
    tex_title = (Texture *) new C2DTexture((const unsigned char *) pfba_title, pfba_title_length);
    font = new C2DFont();
    font->loadFromMemory(pfba_font, pfba_font_length);
#else
    tex_title = new C2DTexture((path + "title.png").c_str());
    font = new C2DFont();
    font->loadFromFile(path + "default.ttf");
#endif
    font->setYOffset(-5);

    // load buttons textures
    buttons = btns;
    for (auto &button : buttons) {
        button.path = path + "buttons/" + std::to_string(button.id) + ".png";
    }
}

C2DUISkin::Button *C2DUISkin::getButton(int id) {

    for (unsigned int i = 0; i < buttons.size(); i++) {
        if (buttons[i].id == id) {
            return &buttons[i];
        }
    }
    return nullptr;
}

C2DUISkin::~C2DUISkin() {

    if (font) {
        delete (font);
    }

    buttons.clear();
}
