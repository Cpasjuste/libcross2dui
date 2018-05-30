//
// Created by cpasjuste on 04/02/18.
//

#ifndef PFBA_GUI_PROGRESSBOX_H
#define PFBA_GUI_PROGRESSBOX_H

//class C2DUIGuiMain;

namespace c2dui {

    class C2DUIGuiProgressBox : public c2d::Rectangle {

    public:

        C2DUIGuiProgressBox(C2DUIGuiMain *gui);

        void setTitle(std::string title);

        void setMessage(std::string message);

        void setProgress(float progress);

    private:

        c2d::Text *title;
        c2d::Text *message;
        c2d::Rectangle *progress_bg;
        c2d::Rectangle *progress_fg;
    };
}

#endif //PFBA_GUI_PROGRESSBOX_H
