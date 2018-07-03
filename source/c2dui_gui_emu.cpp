//
// Created by cpasjuste on 03/02/18.
//

#include "c2dui.h"

using namespace c2d;
using namespace c2dui;

C2DUIGuiEmu::C2DUIGuiEmu(C2DUIGuiMain *u) : Rectangle(u->getRenderer()->getSize()) {

    printf("C2DUIGuiEmu()\n");

    ui = u;
    setFillColor(Color::Transparent);

    fpsText = new Text("0123456789", *ui->getSkin()->font, (unsigned int) ui->getFontSize());
    fpsText->setString("FPS: 00/60");
    fpsText->setPosition(16, 16);
    fpsText->setVisibility(Visibility::Hidden);
    add(fpsText);

    setVisibility(Hidden);

    printf("C2DUIGuiEmu()\n");
}

int C2DUIGuiEmu::run(C2DUIRomList::Rom *rom) {

    printf("C2DUIGuiEmu::run()\n");

    // set fps text on top
    getFpsText()->setLayer(1);

    setVisibility(Visible);
    getUi()->getUiProgressBox()->setVisibility(Hidden);
    getUi()->getUiRomList()->setVisibility(Hidden);

    // set per rom input configuration
    getUi()->updateInputMapping(true);

    resume();

    return 0;
}

void C2DUIGuiEmu::stop() {

    printf("C2DUIGuiEmu::stop()\n");

    if (ui->getAudio()) {
        ui->getAudio()->pause(1);
    }

    if (video) {
        printf("Closing video...\n");
        delete (video);
        video = nullptr;
        printf("Done\n");
    }

    ui->updateInputMapping(false);
    setVisibility(Hidden);
}

void C2DUIGuiEmu::pause() {

    printf("C2DUIGuiEmu::pause()\n");

    paused = true;
    if (ui->getAudio()) {
        ui->getAudio()->pause(1);
    }
    ui->updateInputMapping(false);
#ifdef __SWITCH__
    // restore ui rotation and scaling
    video->clear();
    gfxConfigureTransform(NATIVE_WINDOW_TRANSFORM_FLIP_V);
    gfxConfigureResolution(0, 0);
#endif
}

void C2DUIGuiEmu::resume() {

    printf("C2DUIGuiEmu::resume()\n");

    ui->updateInputMapping(true);

    if (ui->getAudio()) {
        ui->getAudio()->pause(0);
    }

    ui->getRenderer()->clear();
#ifdef __SWITCH__
    // restore game rotation and scaling
    video->updateScaling();
#endif

    paused = false;
}

void C2DUIGuiEmu::updateFb() {

}

void C2DUIGuiEmu::renderFrame(bool draw, int drawFps, float fps) {

}

void C2DUIGuiEmu::updateFrame() {

}

int C2DUIGuiEmu::update() {

    return 0;
}

C2DUIGuiMain *C2DUIGuiEmu::getUi() {
    return ui;
}

C2DUIVideo *C2DUIGuiEmu::getVideo() {
    return video;
}

void C2DUIGuiEmu::setVideo(C2DUIVideo *v) {
    video = v;
    add(video);
}

float C2DUIGuiEmu::getFrameDuration() {
    return frameDuration;
}

void C2DUIGuiEmu::setFrameDuration(float f) {
    frameDuration = f;
}

c2d::Text *C2DUIGuiEmu::getFpsText() {
    return fpsText;
}

char *C2DUIGuiEmu::getFpsString() {
    return fpsString;
}

bool C2DUIGuiEmu::isPaused() {
    return paused;
}
