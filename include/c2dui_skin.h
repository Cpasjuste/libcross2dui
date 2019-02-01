//
// Created by cpasjuste on 23/11/16.
//

#ifndef C2DUI_SKIN_H_
#define C2DUI_SKIN_H_

#include <string>
#include <vector>

namespace c2dui {

    class Skin {

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

        Skin(UIMain *ui, const std::vector<Button> &buttons);

        ~Skin();

        c2d::config::Config *getConfig();

        void loadRectangleShape(c2d::RectangleShape *shape, const std::string &groupName);

        Button *getButton(int id);

        std::string path;
        c2d::Texture *tex_title = nullptr;
        c2d::Font *font = nullptr;
        bool font_available = true;
        std::vector<Button> buttons;

    private:

        c2d::config::Group createRectangleShapeGroup(const std::string &name,
                                                     const std::string &texture,
                                                     const c2d::Color &color,
                                                     const c2d::Color &outlineColor, int outlineSize);

        UIMain *ui = nullptr;
        c2d::config::Config *config = nullptr;
    };
}

#endif //C2DUI_SKIN_H_
