//
// Created by cpasjuste on 16/02/17.
//

#include "c2dui_option.h"
#include "c2dui_option_menu.h"

using namespace c2dui;

C2DUIOptionMenu::C2DUIOptionMenu(C2DUIOptionMenu *parent, std::vector<C2DUIOption> *options, bool isRomCfg) {

    this->parent = parent;

    if (options == nullptr) {
        return;
    }

    C2DUIOptionMenu *menu = this;

    for (unsigned int i = 0; i < options->size(); i++) {

        C2DUIOption option = options->at(i);

        if (option.flags & C2DUIOption::Type::HIDDEN) {
            continue;
        }

        if (option.flags & C2DUIOption::Type::MENU) {
            //printf("NEW MENU: %s (%i)\n", option.getName(), option.index);
            if (i == 0 && !isRomCfg) {
                menu->title = option.getName();
            } else {
                menu = new C2DUIOptionMenu(this, nullptr);
                menu->title = option.getName();
                childs.push_back(menu);
            }
        } else {
            //printf("NEW OPTION: %s (%i)\n", option.getName(), option.index);
            menu->option_ids.push_back(option.index);
        }
    }
}

void C2DUIOptionMenu::addChild(const std::string &title) {

    auto *m = new C2DUIOptionMenu(nullptr, nullptr);
    m->title = title;
    childs.push_back(m);
}

C2DUIOptionMenu::~C2DUIOptionMenu() {

    for (auto &child : childs) {
        delete child;
    }

    childs.clear();
    option_ids.clear();
}
