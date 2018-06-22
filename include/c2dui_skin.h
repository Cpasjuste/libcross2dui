//
// Created by cpasjuste on 23/11/16.
//

#ifndef _SKIN_H_
#define _SKIN_H_

#include <string>
#include <vector>

namespace c2dui {

    class C2DUISkin {

    public:

        struct Button {
            Button(int i, const std::string &n) {
                id = i;
                name = n;
            }

            std::string name;
            std::string path;
            int id = -1;
        };

        C2DUISkin(const std::string &path, const std::vector<Button> &btns);

        ~C2DUISkin();

        Button *getButton(int id);

        std::string path;
        c2d::Texture *tex_title = NULL;
        c2d::Font *font = NULL;
        std::vector<Button> buttons;
    };
}

#endif //_SKIN_H
