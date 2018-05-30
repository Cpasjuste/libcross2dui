//
// Created by cpasjuste on 16/02/17.
//

#ifndef PFBA_MENU_H
#define PFBA_MENU_H

#include <vector>

namespace c2dui {

    class C2DUIOptionMenu {

    public:

        C2DUIOptionMenu(C2DUIOptionMenu *parent, std::vector<C2DUIOption> *options, bool isRomCfg = false);

        ~C2DUIOptionMenu();

        void addChild(const std::string &title);

        std::string title;
        std::vector<int> option_ids;
        std::vector<C2DUIOptionMenu *> childs;
        C2DUIOptionMenu *parent = NULL;

    };
}

#endif //PFBA_MENU_H
