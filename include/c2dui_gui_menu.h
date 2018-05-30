//
// Created by cpasjuste on 30/01/18.
//

#ifndef PFBA_GUI_MENU_H
#define PFBA_GUI_MENU_H

class MenuLine;

namespace c2dui {

    class C2DUIGuiMenu : public c2d::Rectangle {

    public:

        C2DUIGuiMenu(C2DUIGuiMain *ui);

        ~C2DUIGuiMenu();

        int update();

        void load(bool isRomMenu = false, C2DUIOptionMenu *om = NULL);

    private:

        void updateHighlight();

        bool isOptionHidden(C2DUIOption *option);

        C2DUIGuiMain *ui = NULL;
        c2d::Text *title = NULL;
        c2d::Rectangle *highlight = NULL;
        std::vector<MenuLine *> lines;
        c2d::Color fillColor[2] = {
                {55, 55, 55, 230},
                {55, 55, 55, 180}};

        C2DUIOptionMenu *optionMenuGui = NULL;
        C2DUIOptionMenu *optionMenuRom = NULL;
        C2DUIOptionMenu *optionMenu = NULL;
        std::vector <C2DUIOption> *options;
        int optionIndex = 0;
        int optionCount = 0;
        bool isRomMenu = false;
        bool isEmuRunning = false;
    };
}

#endif //PFBA_GUI_MENU_H
