//
// Created by cpasjuste on 16/02/18.
//

#ifndef C2DUI_MENU_STATE_H
#define C2DUI_MENU_STATE_H

class UIStateList;

namespace c2dui {

    class UIStateMenu : public c2d::Rectangle {

    public:

        UIStateMenu(UIMain *ui);

        void show();

        void hide();

        int update();

    private:

        UIMain *ui = NULL;
        UIStateList *uiStateList = NULL;
        c2d::Text *title = NULL;
        bool isEmuRunning = false;
    };
}

#endif //C2DUI_MENU_STATE_H
