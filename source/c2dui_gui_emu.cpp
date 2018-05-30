//
// Created by cpasjuste on 03/02/18.
//

#include "c2dui.h"

// TODO: REMOVE PFBA CODE
#include "../pfba_test/burn.h"
#include "../pfba_test/burner_sdl.h"

using namespace c2d;
using namespace c2dui;

extern int InpMake(Input::Player *players);

extern unsigned char inputServiceSwitch;
extern unsigned char inputP1P2Switch;

C2DUIGuiEmu::C2DUIGuiEmu(C2DUIGuiMain *u) : Rectangle(u->getRenderer()->getSize()) {

    ui = u;
    setFillColor(Color::Transparent);

    fpsText = new Text("0123456789", *ui->getSkin()->font, (unsigned int) ui->getFontSize());
    fpsText->setString("FPS: 00/60");
    fpsText->setPosition(16, 16);
    add(fpsText);

    setVisibility(Hidden);
}

int C2DUIGuiEmu::run(int driver) {

    bForce60Hz = true;
/*
#if defined(__PSP2__) || defined(__RPI__)
    nSekCpuCore = getSekCpuCore();
#endif
*/
    ///////////
    // AUDIO
    //////////
    int audio = ui->getConfig()->getValue(C2DUIOption::Index::ROM_AUDIO, true);
    if (audio && ui->getAudio()->available) {
        printf("Creating audio device...\n");
        // disable interpolation as it produce "cracking" sound
        // on some games (cps1 (SF2), cave ...)
        nInterpolation = 1;
        nFMInterpolation = 0;
        nBurnSoundRate = ui->getAudio()->frequency;
        nBurnSoundLen = ui->getAudio()->buffer_len;
        pBurnSoundOut = ui->getAudio()->buffer;
        ui->getAudio()->reset();
        printf("done\n");
    }

    ///////////
    // DRIVER
    //////////
    printf("InpInit...\n");
    InpInit();
    printf("done\n");
    printf("InpDIP...\n");
    InpDIP();
    printf("done\n");

    printf("Initialize driver...\n");
    if (DrvInit(driver, false) != 0) {
        printf("\nDriver initialisation failed! Likely causes are:\n"
               "- Corrupt/Missing ROM(s)\n"
               "- I/O Error\n"
               "- Memory error\n\n");
        DrvExit();
        InpExit();
        ui->getUiProgressBox()->setVisibility(Hidden);
        ui->getUiMessageBox()->show("ERROR", "DRIVER INIT FAILED", "OK");
        return -1;
    }
    printf("done\n");

    ///////////
    // VIDEO
    //////////
    printf("Creating video device...\n");
    ui->getRenderer()->clear();
    int w, h;
    BurnDrvGetFullSize(&w, &h);
    video = new C2DUIVideo(ui, Vector2f(w, h));
    add(video);
    printf("done\n");

    // set fps text on top
    fpsText->setLayer(1);

    setVisibility(Visible);
    ui->getUiProgressBox()->setVisibility(Hidden);
    ui->getUiRomList()->setVisibility(Hidden);

    // set per rom input configuration
    ui->updateInputMapping(true);

    // reset
    paused = false;
    nFramesEmulated = 0;
    nFramesRendered = 0;
    nCurrentFrame = 0;
    frame_duration = 1.0f / ((float) nBurnFPS / 100.0f);

    return 0;
}

void C2DUIGuiEmu::stop() {

    printf("DrvExit...\n");
    DrvExit();
    printf("Done\n");
    printf("InpExit...\n");
    InpExit();
    printf("Done\n");

    if (ui->getAudio()) {
        ui->getAudio()->pause(1);
    }

    if (video) {
        printf("Closing video...\n");
        delete (video);
        video = NULL;
        printf("Done\n");
    }

    ui->updateInputMapping(false);
    setVisibility(Hidden);
}

void C2DUIGuiEmu::pause() {

    paused = true;
    if (ui->getAudio()) {
        ui->getAudio()->pause(1);
    }
    ui->updateInputMapping(false);
#ifdef __NX__
    // restore ui rotation and scaling
    video->clear();
    gfxSetMode(GfxMode_TiledDouble);
    gfxConfigureTransform(NATIVE_WINDOW_TRANSFORM_FLIP_V);
    gfxConfigureResolution(0, 0);
#endif
}

void C2DUIGuiEmu::resume() {

    ui->updateInputMapping(true);
    if (ui->getAudio()) {
        ui->getAudio()->pause(0);
    }
    paused = false;
    ui->getRenderer()->clear();
#ifdef __NX__
    // restore game rotation and scaling
    video->updateScaling();
#endif
}

void C2DUIGuiEmu::updateFb() {

    if (pBurnDraw == nullptr) {
        nFramesEmulated++;
        nCurrentFrame++;
        nFramesRendered++;
        video->lock(NULL, (void **) &pBurnDraw, &nBurnPitch);
        BurnDrvFrame();
        video->unlock();
    }
}

void C2DUIGuiEmu::renderFrame(bool bDraw, int bDrawFps, float fps) {

    fpsText->setVisibility(
            bDrawFps ? Visible : Hidden);

    if (!paused) {

        nFramesEmulated++;
        nCurrentFrame++;

        pBurnDraw = NULL;
        if (bDraw) {
            nFramesRendered++;
            video->lock(NULL, (void **) &pBurnDraw, &nBurnPitch);
        }
        BurnDrvFrame();
        if (bDraw) {
            video->unlock();
        }

        if (bDrawFps) {
            sprintf(fpsString, "FPS: %.2g/%2d", fps, (nBurnFPS / 100));
            fpsText->setString(fpsString);
        }

        if (ui->getAudio() && ui->getAudio()->available) {
            ui->getAudio()->play();
        }
    }
}

float timer = 0;

void C2DUIGuiEmu::updateFrame() {

    int showFps = ui->getConfig()->getValue(C2DUIOption::Index::ROM_SHOW_FPS, true);
    int frameSkip = ui->getConfig()->getValue(C2DUIOption::Index::ROM_FRAMESKIP, true);

    if (frameSkip) {
        bool draw = nFramesEmulated % (frameSkip + 1) == 0;
        renderFrame(draw, showFps, ui->getRenderer()->getFps());
#ifdef __NX__
        ui->getRenderer()->flip(false);
#else
        ui->getRenderer()->flip(draw);
#endif
        float delta = ui->getRenderer()->getDeltaTime().asSeconds();
        if (delta < frame_duration) { // limit fps
            //printf("f: %f | d: %f | m: %f | s: %i\n", frame_duration, delta, frame_duration - delta,
            //       (unsigned int) ((frame_duration - delta) * 1000));
            ui->getRenderer()->delay((unsigned int) ((frame_duration - delta) * 1000));
        }
    } else {
        renderFrame(true, showFps, ui->getRenderer()->getFps());
#ifdef __NX__
        ui->getRenderer()->flip(false);
#else
        ui->getRenderer()->flip();
#endif
        /*
        timer += ui->getRenderer()->getDeltaTime().asSeconds();
        if (timer >= 1) {
            timer = 0;
            printf("fps: %.2g/%2d, delta: %f\n", ui->getRenderer()->getFps(), (nBurnFPS / 100),
                   ui->getRenderer()->getDeltaTime().asSeconds());
        }
        */
    }
}

int C2DUIGuiEmu::update() {

    inputServiceSwitch = 0;
    inputP1P2Switch = 0;

    int rotation_config =
            ui->getConfig()->getValue(C2DUIOption::Index::ROM_ROTATION, true);
    int rotate_input = 0;
#ifdef __PSP2__
    // TODO: find a way to unify platforms,
    // or allow rotation config from main.cpp
    if (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL) {
        if (rotation_config == 0) {
            //rotate controls by 90 degrees
            rotate_input = 1;
        } else if (rotation_config == 2) {
            //rotate controls by 270 degrees
            rotate_input = 3;
        }
    }
#elif __SWITCH__
    if (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL) {
        if (rotation_config == 0) {             // OFF
            //rotate controls by 270 degrees
            rotate_input = 3;
        } else if (rotation_config == 1) {      // ON
            //rotate controls by 270 degrees
            rotate_input = 0;
        } else if (rotation_config == 2) {      // FLIP
            //rotate controls by 270 degrees
            rotate_input = 1;
        }
    }
#else
    if (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL) {
        rotate_input = rotation_config ? 0 : 3;
    }
#endif

    Input::Player *players = ui->getInput()->update(rotate_input);

    // process menu
    if ((players[0].state & Input::Key::KEY_START)
        && (players[0].state & Input::Key::KEY_COIN)) {
        pause();
        return UI_KEY_SHOW_MEMU_ROM;
    } else if ((players[0].state & Input::Key::KEY_START)
               && (players[0].state & Input::Key::KEY_FIRE5)) {
        pause();
        return UI_KEY_SHOW_MEMU_STATE;
    } else if ((players[0].state & Input::Key::KEY_START)
               && (players[0].state & Input::Key::KEY_FIRE3)) {
        inputServiceSwitch = 1;
    } else if ((players[0].state & Input::Key::KEY_START)
               && (players[0].state & Input::Key::KEY_FIRE4)) {
        inputP1P2Switch = 1;
    } else if (players[0].state & EV_RESIZE) {
        // useful for sdl resize event for example
        video->updateScaling();
    }

    InpMake(players);
    updateFrame();

    return 0;
}

C2DUIVideo *C2DUIGuiEmu::getVideo() {
    return video;
}
