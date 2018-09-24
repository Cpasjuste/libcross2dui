//
// Created by cpasjuste on 30/01/18.
//

#ifndef C2DUI_GUI_MENU_H
#define C2DUI_GUI_MENU_H

class MenuLine;

namespace c2dui {

    class C2DUIGuiMenu : public c2d::Rectangle {

    public:

        C2DUIGuiMenu(C2DUIGuiMain *ui);

        ~C2DUIGuiMenu();

        int update();

        void load(bool isRomMenu = false, C2DUIOptionMenu *om = NULL);

        C2DUIGuiMain *getUi();

        bool isRom();

        virtual bool isOptionHidden(C2DUIOption *option);

    private:

        void updateHighlight();

        C2DUIGuiMain *ui = nullptr;
        c2d::Text *title = nullptr;
        c2d::Rectangle *highlight = nullptr;
        std::vector<MenuLine *> lines;
        c2d::Color fillColor[2] = {
                {55, 55, 55, 230},
                {55, 55, 55, 180}};

        C2DUIOptionMenu *optionMenuGui = nullptr;
        C2DUIOptionMenu *optionMenuRom = nullptr;
        C2DUIOptionMenu *optionMenu = nullptr;
        std::vector <C2DUIOption> *options;
        c2d::TweenPosition *tweenPosition;
        c2d::TweenAlpha *tweenAlpha;
        int optionIndex = 0;
        int optionCount = 0;
        bool isRomMenu = false;
        bool isEmuRunning = false;
    };
}

#endif //C2DUI_GUI_MENU_H
