//
// Created by cpasjuste on 30/01/18.
//

#ifndef C2DUI_UI_MENU_H
#define C2DUI_UI_MENU_H

class MenuLine;

namespace c2dui {

    class UIMenu : public c2d::Rectangle {

    public:

        UIMenu(UIMain *ui);

        ~UIMenu();

        int update();

        void load(bool isRomMenu = false, OptionMenu *om = NULL);

        UIMain *getUi();

        bool isRom();

        virtual bool isOptionHidden(Option *option);

    private:

        void updateHighlight();

        UIMain *ui = nullptr;
        c2d::Text *title = nullptr;
        c2d::Rectangle *highlight = nullptr;
        std::vector<MenuLine *> lines;
        c2d::Color fillColor[2] = {
                {55, 55, 55, 230},
                {55, 55, 55, 180}};

        OptionMenu *optionMenuGui = nullptr;
        OptionMenu *optionMenuRom = nullptr;
        OptionMenu *optionMenu = nullptr;
        std::vector <Option> *options;
        c2d::TweenPosition *tweenPosition;
        int optionIndex = 0;
        int optionCount = 0;
        bool isRomMenu = false;
        bool isEmuRunning = false;
    };
}

#endif //C2DUI_UI_MENU_H
