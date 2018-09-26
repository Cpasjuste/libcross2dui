//
// Created by cpasjuste on 03/02/18.
//

#ifdef __SWITCH__
#include <switch.h>
#endif

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
}

void C2DUIGuiEmu::addAudio(int rate, int fps, Audio::C2DAudioCallback cb) {

    if (audio) {
        delete (audio);
        audio = nullptr;
    }

    audio = new C2DAudio(rate, fps, cb);
}

void C2DUIGuiEmu::addVideo(C2DUIGuiMain *ui, void **pixels, int *pitch, const c2d::Vector2f &size, int format) {

    if (video) {
        delete (video);
        video = nullptr;
    }

    video = new C2DUIVideo(ui, pixels, pitch, size, format);
    add(video);
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

void C2DUIGuiEmu::resume() {

    printf("C2DUIGuiEmu::resume()\n");

    ui->updateInputMapping(true);

    if (audio) {
        audio->pause(0);
    }

    ui->getRenderer()->clear();

    paused = false;
}

void C2DUIGuiEmu::stop() {

    printf("C2DUIGuiEmu::stop()\n");

    if (audio) {
        printf("Closing audio...\n");
        audio->pause(1);
        delete (audio);
        audio = nullptr;
    }

    if (video) {
        printf("Closing video...\n");
        delete (video);
        video = nullptr;
    }

    ui->updateInputMapping(false);
    setVisibility(Hidden);
}

void C2DUIGuiEmu::pause() {

    printf("C2DUIGuiEmu::pause()\n");

    paused = true;
    if (audio) {
        audio->pause(1);
    }
    ui->updateInputMapping(false);
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

c2d::Audio *C2DUIGuiEmu::getAudio() {
    return audio;
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

