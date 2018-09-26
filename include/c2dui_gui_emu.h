//
// Created by cpasjuste on 03/02/18.
//

#ifndef C2DUI_GUI_EMU_H
#define C2DUI_GUI_EMU_H

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

        virtual void updateFb() {};

        virtual void updateFrame() {};

        virtual void renderFrame(bool draw = true, int drawFps = false, float fps = 0) {};

        C2DUIGuiMain *getUi();

        C2DUIVideo *getVideo();

        c2d::Audio *getAudio();

        void addAudio(c2d::Audio *audio);

        void addAudio(int rate = 48000, int fps = 60, c2d::Audio::C2DAudioCallback cb = nullptr);

        void addVideo(C2DUIVideo *video);

        void addVideo(C2DUIGuiMain *ui, void **pixels, int *pitch,
                      const c2d::Vector2f &size, int format = C2D_TEXTURE_FMT_RGB565);

        float getFrameDuration();

        void setFrameDuration(float f);

        c2d::Text *getFpsText();

        char *getFpsString();

        bool isPaused();

    private:

        C2DUIGuiMain *ui = nullptr;
        C2DUIVideo *video = nullptr;
        c2d::Audio *audio = nullptr;
        c2d::Text *fpsText = nullptr;
        char fpsString[32];
        bool paused = true;
        float frameDuration = 0;
    };
}

#endif //C2DUI_GUI_EMU_H
