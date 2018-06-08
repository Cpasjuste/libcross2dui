//
// Created by cpasjuste on 03/02/18.
//

#ifndef PFBA_GUI_EMU_H
#define PFBA_GUI_EMU_H

#include <sys/time.h>

namespace c2dui {

    class C2DUIGuiEmu : public c2d::Rectangle {

    public:

        C2DUIGuiEmu(C2DUIGuiMain *ui);

        virtual int run(C2DUIRomList::Rom *rom);

        virtual void stop();

        virtual void pause();

        virtual void resume();

        virtual int update();

        virtual void updateFb();

        virtual void updateFrame();

        virtual void renderFrame(bool draw = true, int drawFps = false, float fps = 0);

        C2DUIGuiMain *getUi();

        C2DUIVideo *getVideo();

        void setVideo(C2DUIVideo *video);

        float getFrameDuration();

        void setFrameDuration(float f);

        c2d::Text *getFpsText();

        char *getFpsString();

        bool isPaused();

    private:

        C2DUIGuiMain *ui = NULL;
        C2DUIVideo *video = NULL;
        c2d::Text *fpsText = NULL;
        char fpsString[32];
        bool paused = true;
        float frameDuration = 0;
    };
}

#endif //PFBA_GUI_EMU_H
