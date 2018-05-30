//
// Created by cpasjuste on 16/02/18.
//

#ifndef PFBA_GUI_STATES_H
#define PFBA_GUI_STATES_H

class C2DUIGuiSaveStateList;

namespace c2dui {

    class C2DUIGuiState : public c2d::Rectangle {

    public:

        C2DUIGuiState(C2DUIGuiMain *ui);

        void load();

        void unload();

        int update();

    private:

        C2DUIGuiMain *ui = NULL;
        C2DUIGuiSaveStateList *uiStateList = NULL;
        c2d::Text *title = NULL;
        bool isEmuRunning = false;
    };
}

#endif //PFBA_GUI_STATES_H
