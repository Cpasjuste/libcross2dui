//
// Created by cpasjuste on 03/02/18.
//

#ifndef PFBA_GUI_EMU_H
#define PFBA_GUI_EMU_H

#include <sys/time.h>

#ifdef __NX__
#define Video NXVideo
#endif

//class C2DUIGuiMain;

namespace c2dui {

    class C2DUIGuiEmu : public c2d::Rectangle {

    public:

        C2DUIGuiEmu(C2DUIGuiMain *ui);

        int run(int driver);

        void stop();

        void pause();

        void resume();

        int update();

        void updateFb();

        C2DUIVideo *getVideo();

    private:

        void updateFrame();

        void renderFrame(bool draw = true, int drawFps = false, float fps = 0);

        C2DUIGuiMain *ui = NULL;
        C2DUIVideo *video = NULL;
        c2d::Text *fpsText = NULL;
        char fpsString[32];
        bool paused = false;
        float frame_duration = 0;
    };
}
#endif //PFBA_GUI_EMU_H
