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

        struct RectangleShapeGroup {
            c2d::FloatRect rect;
            c2d::Origin origin;
            std::string texture;
            c2d::Color color;
            c2d::Color outlineColor;
            int outlineSize;
            bool available = false;
        };

        struct TextGroup {
            std::string text;
            unsigned int size;
            c2d::FloatRect rect;
            c2d::Origin origin;
            c2d::Color color;
            c2d::Color outlineColor;
            int outlineSize;
            bool available = false;
        };

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

        void loadRectangleShape(c2d::RectangleShape *shape, const std::vector<std::string> &tree);

        RectangleShapeGroup getRectangleShape(const std::vector<std::string> &tree);

        void loadText(c2d::Text *text, const std::vector<std::string> &tree);

        TextGroup getText(const std::vector<std::string> &tree);

        Button *getButton(int id);

        c2d::Font *getFont();

        char *getZippedData(const std::string &path, const std::string &name, int *size = nullptr);

        std::string path;
        c2d::Font *font = nullptr;
        bool font_available = true;
        char *font_data = nullptr;
        std::vector<Button> buttons;

    private:

        c2d::config::Group createRectangleShapeGroup(const std::string &name,
                                                     const c2d::FloatRect &rect,
                                                     const c2d::Origin &origin,
                                                     const std::string &texture,
                                                     const c2d::Color &color,
                                                     const c2d::Color &outlineColor, int outlineSize);

        c2d::config::Group createTextGroup(const std::string &name,
                                           int size,
                                           const c2d::FloatRect &rect,
                                           const c2d::Origin &origin,
                                           const c2d::Color &color,
                                           const c2d::Color &outlineColor, int outlineSize);

        UIMain *ui = nullptr;
        c2d::config::Config *config = nullptr;
        bool useZippedSkin = false;
    };
}

#endif //C2DUI_SKIN_H_
